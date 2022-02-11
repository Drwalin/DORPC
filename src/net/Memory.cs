
using System.Runtime.InteropServices;
using System.Buffers.Binary;

namespace net {
	class Memory {
		public static int Read(byte[] buffer, int offset, ref byte ret) {
			ret = buffer[offset]; return 1;
		}
		
		public static int Read(byte[] buffer, int offset, ref short ret) {
			ret = BinaryPrimitives.ReadInt16LittleEndian(
					new Span<byte>(buffer, offset, 2));
			return 2;
		}
		
		public static int Read(byte[] buffer, int offset, ref ushort ret) {
			ret = BinaryPrimitives.ReadUInt16LittleEndian(
					new Span<byte>(buffer, offset, 2));
			return 2;
		}
		
		public static int Read(byte[] buffer, int offset, ref int ret) {
			ret = BinaryPrimitives.ReadInt32LittleEndian(
					new Span<byte>(buffer, offset, 4));
			return 4;
		}
		
		public static int Read(byte[] buffer, int offset, ref uint ret) {
			ret = BinaryPrimitives.ReadUInt32LittleEndian(
					new Span<byte>(buffer, offset, 4));
			return 4;
		}
		
		public static int Read(byte[] buffer, int offset, ref long ret) {
			ret = BinaryPrimitives.ReadInt64LittleEndian(
					new Span<byte>(buffer, offset, 8));
			return 8;
		}
		
		public static int Read(byte[] buffer, int offset, ref ulong ret) {
			ret = BinaryPrimitives.ReadUInt64LittleEndian(
					new Span<byte>(buffer, offset, 8));
			return 8;
		}
		
		public static int Read(byte[] buffer, int offset, ref Half ret) {
			ret = BinaryPrimitives.ReadHalfLittleEndian(
					new Span<byte>(buffer, offset, 2));
			return 2;
		}
		
		public static int Read(byte[] buffer, int offset, ref float ret) {
			ret = BinaryPrimitives.ReadSingleLittleEndian(
					new Span<byte>(buffer, offset, 4));
			return 4;
		}
		
		public static int Read(byte[] buffer, int offset, ref double ret) {
			ret = BinaryPrimitives.ReadDoubleLittleEndian(
					new Span<byte>(buffer, offset, 8));
			return 8;
		}
		
		
		
		
		public static int Write(byte[] buffer, int offset, byte value) {
			buffer[offset] = value; return 1;
		}
		
		public static int Write(byte[] buffer, int offset, short value) {
			BinaryPrimitives.WriteInt16LittleEndian(
					new Span<byte>(buffer, offset, 2), value);
			return 2;
		}
		
		public static int Write(byte[] buffer, int offset, ushort value) {
			BinaryPrimitives.WriteUInt16LittleEndian(
					new Span<byte>(buffer, offset, 2), value);
			return 2;
		}
		
		public static int Write(byte[] buffer, int offset, int value) {
			BinaryPrimitives.WriteInt32LittleEndian(
					new Span<byte>(buffer, offset, 4), value);
			return 4;
		}
		
		public static int Write(byte[] buffer, int offset, uint value) {
			BinaryPrimitives.WriteUInt32LittleEndian(
					new Span<byte>(buffer, offset, 4), value);
			return 4;
		}
		
		public static int Write(byte[] buffer, int offset, long value) {
			BinaryPrimitives.WriteInt64LittleEndian(
					new Span<byte>(buffer, offset, 8), value);
			return 8;
		}
		
		public static int Write(byte[] buffer, int offset, ulong value) {
			BinaryPrimitives.WriteUInt64LittleEndian(
					new Span<byte>(buffer, offset, 8), value);
			return 8;
		}
		
		public static int Write(byte[] buffer, int offset, Half value) {
			BinaryPrimitives.WriteHalfLittleEndian(
					new Span<byte>(buffer, offset, 2), value);
			return 2;
		}
		
		public static int Write(byte[] buffer, int offset, float value) {
			BinaryPrimitives.WriteSingleLittleEndian(
					new Span<byte>(buffer, offset, 4), value);
			return 4;
		}
		
		public static int Write(byte[] buffer, int offset, double value) {
			BinaryPrimitives.WriteDoubleLittleEndian(
					new Span<byte>(buffer, offset, 8), value);
			return 8;
		}
	}
}

