
namespace net {
	class ReceivedMessage {
		public Packet[]? packets;
		public byte[]? messageBuffer;
		public bool complete = false;
		
		
		public ReceivedMessage(Peer peer) {
			this.peer = peer;
		}
		
		public bool ReceiveBuffer(byte[] buffer) {

		}
	}
}

