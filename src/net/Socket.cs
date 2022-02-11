
using System.Net;
using System.Net.Sockets;
using System.Collections.Concurrent;
using System.Threading;

namespace net {
	sealed class Socket {
		public static AddressFamily IPv4 = AddressFamily.InterNetwork;
		public static AddressFamily IPv6 = AddressFamily.InterNetworkV6;
		
		public UdpClient udpv4;
		
		private ConcurrentQueue<Packet> outgoingPackets;
		private Dictionary<IPEndPoint, Peer> peers;
		
		private bool queueRunning, running, stop;
		private Thread? thread;
		
		
		public Socket(int port) {
			udpv4 = new UdpClient(new IPEndPoint(IPAddress.Any, port));
			udpv4.Client.Blocking = false;
			udpv4.DontFragment = false;
			udpv4.Client.ReceiveBufferSize = 1024*1024;
			
			outgoingPackets = new ConcurrentQueue<Packet>();
			stop = false;
			running = false;
			queueRunning = false;
			thread = null;
			
			peers = new Dictionary<IPEndPoint, Peer>();
		}
		
		private void ReceiveOne() {
			IPEndPoint endpoint = new IPEndPoint(IPAddress.Any, 0);
			byte[] buffer = udpv4.Receive(ref endpoint);
			if(buffer != null) {
				if(buffer.Length > 0) {
					ParseMessage(buffer, endpoint);
				}
			}
		}
		
		private void ParseMessage(byte[] buffer, IPEndPoint endpoint) {
			
		}
		
		private void SendOne() {
			Packet? packet;
			if(outgoingPackets.TryDequeue(out packet)) {
				packet.Execute();
			}
		}
		
		
		public void OneRun() {
			for(int i=0; i<16 && udpv4.Available>0; ++i) {
				ReceiveOne();
			}
			for(int i=0; i<16 && outgoingPackets.Count>0; ++i) {
				SendOne();
			}
		}
		
		
		public bool IsRunnging() {
			return running;
		}
		
		public void Loop() {
			running = true;
			while(stop == false) {
				try {
					while(stop == false) {
						OneRun();
						if(!IsBusy())
							Thread.Sleep(1);
					}
				} catch {
				}
			}
			running = false;
		}
		
		public void Start() {
			if(queueRunning==false || running==false) {
				stop = false;
				running = false;
				queueRunning = true;
				thread = new Thread(() => {
						Loop();
						thread = null;
						});
				thread.Start();
				while(running == false) {
					Thread.Sleep(1);
				}
			}
		}
		
		public void Stop() {
			if(running || queueRunning) {
				stop = true;
				thread?.Join();
			}
		}
		
		public void Close() {
			Stop();
			udpv4.Close();
		}
		
		
		public bool IsBusy() {
			return outgoingPackets.Count > 0 || udpv4.Available > 0;
		}
		
		
		public void AddPacket(Packet packet) {
			outgoingPackets.Enqueue(packet);
		}
		
		
	}
}

