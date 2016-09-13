/*
           n6
           |
   n0--n1--n2--n3--n4--n5
                   |
                   n7 
*/

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TTLExceeded");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_INFO);
  LogComponentEnable ("TTLExceeded", LOG_LEVEL_INFO);
  NS_LOG_INFO("Creating nodes...");
 CommandLine cmd;


  cmd.Parse (argc,argv);
  NodeContainer nodes;
  nodes.Create (8);

  NodeContainer net1(nodes.Get(0), nodes.Get(1));
  NodeContainer net2(nodes.Get(1), nodes.Get(2));
  NodeContainer net3(nodes.Get(2), nodes.Get(3));
  NodeContainer net4(nodes.Get(3), nodes.Get(4));
  NodeContainer net5(nodes.Get(4), nodes.Get(5));
  NodeContainer net6(nodes.Get(2), nodes.Get(6));
  NodeContainer net7(nodes.Get(4), nodes.Get(7));
     
  
  NS_LOG_INFO("Creating channels...");


  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer d1, d2, d3, d4, d5, d6, d7;
  d1 = pointToPoint.Install (net1);
  d2 = pointToPoint.Install (net2);
  d3 = pointToPoint.Install (net3);
  d4 = pointToPoint.Install (net4);
  d5 = pointToPoint.Install (net5);
  d6 = pointToPoint.Install (net6);
  d7 = pointToPoint.Install (net7);


  InternetStackHelper internet;
  internet.Install (nodes);

  NS_LOG_INFO("Assigning IP Addresses...");

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i1 = ipv4.Assign (d1);


  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i2 = ipv4.Assign (d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i3 = ipv4.Assign (d3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i4 = ipv4.Assign (d4);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i5 = ipv4.Assign (d5);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i6 = ipv4.Assign (d6);

  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i7 = ipv4.Assign (d7);



  NS_LOG_INFO("Creating Applications...");

  uint16_t port = 9090;
  BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(i5.GetAddress(1), port)); 

  source.SetAttribute ("MaxBytes", UintegerValue (200));
  
  ApplicationContainer sourceApps = source.Install (nodes.Get (0));
  ApplicationContainer sourceApps2 = source.Install (nodes.Get (6));  
    

  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (Seconds (10.0));

  sourceApps2.Start (Seconds (0.0));
  sourceApps2.Stop (Seconds (10.0));  

  PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = sink.Install (nodes.Get (5));
  ApplicationContainer sinkApps2 = sink.Install (nodes.Get (7));
  

  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (10.0));

  sinkApps2.Start (Seconds (0.0));
  sinkApps2.Stop (Seconds (10.0));

  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("ttl-exceeded.tr"));
  pointToPoint.EnablePcapAll("ttl-exceeded", false);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  NS_LOG_INFO("Running simulation...");

  Simulator::Stop(Seconds(10.00));

  Simulator::Run ();
  Simulator::Destroy ();
 
  NS_LOG_INFO("Done.");

  Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(sinkApps.Get(0));
  std::cout<<"Total Bytes Received: "<<sink1->GetTotalRx()<<std::endl;
  return 0;
}
