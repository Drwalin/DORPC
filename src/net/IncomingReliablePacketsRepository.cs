
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;

namespace net {
	sealed class IncomingReliablePacketsRepository {
		private Dictionary<ulong, ReceivedMessage> packets;
		private Peer peer;
		
		public IncomingReliablePacketsRepository(Peer peer) {
			packets = new Dictionary<ulong, Packet>();
			this.peer = peer;
		}
		
		public bool PushPacket(byte[] buffer) {
		}
	}
}

