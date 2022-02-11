
using System.Net;
using System.Net.Sockets;
using System.Collections.Concurrent;
using System.Threading;

namespace net {
	sealed class Peer {
		public const int UNRELIABLE_HEADER_LENGTH = 1; // TYPE
		public const int RELIABLE_HEADER_LENGTH = 1+4+2+2; // TYPE + SEQUENCE + TOTAL_LENGTH + OFFSET
		public const int ENCRYPTED_HEADER_LENGTH =
			Packet.IV_SIZE+Packet.TAG_SIZE;
		
		public const int IV_SIZE = 12;
		public const int TAG_SIZE = 16;
		
		public const ushort MAX_RELIABLE_MESSAGE_SIZE = ushort.MaxValue;

			
		public Socket socket;
		public UdpClient udp;
		public IPEndPoint endpoint;
		
		
		public int mtu = 1024; // TODO: 768? 1500? 1500-(ipv4/6.header+udp.header)?
							   // TODO: mtu discovery
		
		public int usRtt = 50; // micro seconds round trip time (ping)
		private const int usRttRounds = 16;
		private int usRttSum;
		private int[] usRttRound = new int[usRttRounds];
		private int usRttSeq = 0;
		
		
		public Peer(Socket socket, IPEndPoint endpoint) {
			this.endpoint = endpoint;
			this.socket = socket;
			for(int i=0; i<usRttRound.Length; ++i) {
				usRttSum += usRttRound[i] = 50;
			}
			if(endpoint.AddressFamily == Socket.IPv4) {
				udp = socket.udpv4; 
			} else {
				throw new Exception("IPv6 is not implemented yet.");
				// TODO:
				// udp = socket.udpv6;
				// Add IPv6
			}
			SetMtu(1024); // TODO: better default value? MTU discovery?
		}
		

		public void PushPingResult(int value) {
			usRttSeq++;
			usRttSeq &= usRttRounds-1;
			usRttSum -= usRttRound[usRttSeq];
			usRttRound[usRttSeq] = value;
			usRttSum += value;
			usRtt = usRttSum>>4;
		}
		
		public void SetMtu(int newMtu) {
			if(mtu != newMtu) {
				mtu = newMtu;
			}
		}
		
		
		public void Send(byte[] buffer, PacketType type) {
		}
		
		public void SendReliable(byte[] buffer) {
			
		}
		
		public bool SendUnreliable(byte[] buffer) {
			if(buffer.Length > GetMaxSinglePacketData(false))
				return false;
			Packet packet = new Packet(this, PacketType.UNRELIABLE,
					buffer, 0, buffer.Length);
			socket.AddPacket(packet);
			return true;
		}
		
		
		
		public bool Encrypt(
				ReadOnlySpan<byte> plaintext,
				ReadOnlySpan<byte> iv,
				Span<byte> tag,
				Span<byte> ciphertext,
				ReadOnlySpan<byte> secondTag) {
			throw new Exception("Peer.Encrypt Not implemented yet.");
			return false;
		}
		public bool Decrypt(
				ReadOnlySpan<byte> ciphertext,
				ReadOnlySpan<byte> iv,
				ReadOnlySpan<byte> tag,
				Span<byte> plaintext,
				ReadOnlySpan<byte> secondTag) {
			throw new Exception("Peer.Decrypt Not implemented yet.");
			return false;
		}
		
		public int GetMaxSinglePacketData(bool reliable) {
			// TODO: if?encrypted
			if(reliable) {
				return mtu - RELIABLE_HEADER_LENGTH;
			} else {
				return mtu - UNRELIABLE_HEADER_LENGTH;
			}
		}
	}
}

