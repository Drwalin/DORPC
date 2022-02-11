
using System;
using System.Net;
using System.Threading;
using System.Security.Cryptography;

namespace net {
	enum PacketType : byte {
		CERTIFICATE = 0x01,
		CERTIFICATE_REQUEST = 0x02,
		KEX = 0x03,
		KEX_REQUEST = 0x04,
		
		MTU_DISCOVERY = 0x05,
		
		PING = 0x06,
		PONG = 0x07,
		
		ACK_PAKCETS = 0x0A,
		
		
		_MESSAGE_SINGLEPACKET = 0x08,
		_MESSAGE_MULTIPACKET = 0x09,
		
		UNRELIABLE = _MESSAGE_SINGLEPACKET,
		RELIABLE = _MESSAGE_SINGLEPACKET | _RELIABLE,
		ENCRYPTED_UNRELIABLE = _MESSAGE_SINGLEPACKET | _ENCRYPTED,
		ENCRYPTED_RELIABLE = _MESSAGE_SINGLEPACKET | _ENCRYPTED | _RELIABLE,
		
		// Flags:
		_ENCRYPTED = 0x80,
		_RELIABLE = 0x40,
		// TODO:
		// _ORDERED = 0x20,
		_HEADER = 0x10
	}
	
	struct ReliablePacketHeader {
		public PacketType type;
		public int sequenceNumber = 0;
		public byte fragmentsCount;
		public byte fragmentId;
		public ushort totalLength;
	}
	
	sealed class Packet {
		public Peer peer;
		public ReliablePacketHeader header;
		
		public byte[] source;
		public int sourceOffset;
		
		public byte[]? buffer;
		public ushort length;		// source data size
		public ushort bufferSize;	// buffer
		
		public const int IV_SIZE = Peer.IV_SIZE;
		public const int TAG_SIZE = Peer.TAG_SIZE;
		
		public const int UNRELIABLE_HEADER_LENGTH =
			Peer.UNRELIABLE_HEADER_LENGTH;
		public const int RELIABLE_HEADER_LENGTH =
			Peer.RELIABLE_HEADER_LENGTH;
		public const int ENCRYPTED_HEADER_LENGTH =
			Peer.ENCRYPTED_HEADER_LENGTH;
		
		
		public Packet(Peer peer, PacketType type, byte[] source, int offset,
				int length, byte fragmentsCount, byte fragmentId,
				ushort otherPacketsDataSize) {
			this.peer = peer;
			this.source = source;
			this.length = length;
			header.type = type;
			if(source.Length > ushort.MaxValue)
				throw new Exception("Too big source buffer for net.Packet");
			header.totalLength = (ushort)source.Length;
			header.offset = offset;
			if(type  == PacketType.RELIABLE
					|| type == PacketType.ENCRYPTED_RELIABLE ) {
				header.sequenceNumber = GetNextSequenceNumber();
			}
			MakeBufferWithHeader();
		}
		
		public void MakeBufferWithHeader() {
			bufferSize = length;
			bufferSize += 1;
			if((header.type & PacketType._RELIABLE) == PacketType._RELIABLE)
				bufferSize += 4+2+2;
			if((header.type & PacketType._ENCRYPTED) == PacketType._ENCRYPTED)
				bufferSize += IV_SIZE+TAG_SIZE;
			buffer = new byte[bufferSize];
			buffer[bufferSize-1] = (byte)header.type;
			if((header.type & PacketType._RELIABLE) == PacketType._RELIABLE) {
				BitConverter.TryWriteBytes(
						new Span<byte>(buffer, GetSeqNumOffset(), 4),
						header.sequenceNumber);
				BitConverter.TryWriteBytes(
						new Span<byte>(buffer, GetTotalSizeOffset(), 2),
						header.totalLength);
				BitConverter.TryWriteBytes(
						new Span<byte>(buffer, GetOffsetOffset(), 2),
						header.offset);
			}
			FillDataBuffer();
		}
		
		public void FillDataBuffer() {
			if(buffer == null)
				return;
			if((header.type & PacketType._ENCRYPTED) == PacketType._ENCRYPTED) {
				RandomNumberGenerator.Fill(new Span<byte>(buffer, GetIVOffset(),
							IV_SIZE));
				bool reliable = ((header.type & PacketType._RELIABLE)
					== PacketType._RELIABLE);
				peer.Encrypt(
						new ReadOnlySpan<byte>(source, header.offset, length),
						new ReadOnlySpan<byte>(buffer, GetIVOffset(), IV_SIZE),
						new Span<byte>(buffer, GetTagOffset(), TAG_SIZE),
						new Span<byte>(buffer, GetDataOffset(), length),
						new ReadOnlySpan<byte>(buffer,
							reliable ? GetSeqNumOffset() : GetTypeOffset(),
							reliable ? RELIABLE_HEADER_LENGTH :
								UNRELIABLE_HEADER_LENGTH)
							);
			} else {
				Buffer.BlockCopy(source, header.offset, buffer, GetDataOffset(),
						length);
			}
		}
		
		
		public Packet(Peer peer, byte[] receivedPacket,
				ReceivedMessage message) {

		}
		
		
		
		
		public void Execute() {
			if(buffer == null)
				MakeBufferWithHeader();
			if(buffer != null)
				peer.udp.Send(buffer, bufferSize, peer.endpoint);
		}
		
		
		public int GetIVOffset() {
			return 0;
		}
		
		public int GetTagOffset() {
			if((header.type & PacketType._ENCRYPTED) == PacketType._ENCRYPTED)
				return IV_SIZE;
			return 0;
		}
		
		public int GetDataOffset() {
			if((header.type & PacketType._ENCRYPTED) == PacketType._ENCRYPTED)
				return IV_SIZE+TAG_SIZE;
			return 0;
		}
		
		public int GetTypeOffset() {
			return bufferSize-1-1;
		}
		
		public int GetSeqNumOffset() {
			return GetTypeOffset()-4;
		}
		
// 		public int GetTotalSizeOffset() {
// 			return GetSeqNumOffset()-2;
// 		}
// 		
// 		public int GetOffsetOffset() {
// 			return GetTotalSizeOffset()-2;
// 		}
		
		
		private static int sequenceNumberCounter = 1;
		public static int GetNextSequenceNumber() {
			int value = Interlocked.Increment(ref sequenceNumberCounter);
			if(value != 0)
				return value;
			return GetNextSequenceNumber();
		}
		public static bool IsOrderedSequenceNumber(int a, int b) {
			if(b > 0) {
				if(a > 0) {
					return b > a;
				} else {
					if(b > (1<<30) && a < -(1<<30)) {
						return false;
					} else {
						return b > a;
					}
				}
			} else {
				if(a > 0) {
					if(a > (1<<30) && b < -(1<<30)) {
						return true;
					} else {
						return a > b;
					}
				} else {
					return b > a;
				}
			}
		}
	}
}
