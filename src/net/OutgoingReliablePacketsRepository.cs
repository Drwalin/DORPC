
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;

namespace net {
	sealed class OutgoingReliablePacketsRepository {
		private Dictionary<ulong, Packet> packets;
		
		public OutgoingReliablePacketsRepository() {
			packets = new Dictionary<ulong, Packet>();
		}
	}
}

