//sudo setcap cap_net_raw,cap_net_admin=eip mac_daq

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <getopt.h>

#include <signal.h>  /*for signal() and raise()*/
 
#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <math.h>
#include <time.h>

#include <bitset>

// // ROOT includes
// #include "TFile.h"
// #include "TDirectory.h"
// #include "TNtuple.h"
// #include "TCanvas.h"
// #include "TGraph.h"
// #include "TH1.h"



#include <pcap.h> 
#include <sys/socket.h>
#include "EthernetCap.h"

using namespace std;


// EthernetCap *p_ecap;

PcapDev::PcapDev(){
  pcap_if_t *alldevs;
  pcap_if_t *d;
  char err_buf[PCAP_ERRBUF_SIZE];   
  char packet_filter[]="ether src ff:ff:ff:c7:05:01"; //filter expression
  bpf_u_int32 netmask=0xffffff; 
  struct bpf_program fcode; // compile filter program  
  int i = 0;

  if (pcap_findalldevs(&alldevs, err_buf) == -1)
  {
      cerr <<"Error in pcap_findalldevs_ex:"<< err_buf << endl;
      exit(1);
  }
  for(d= alldevs; d != NULL; d= d->next)
  {
      cout << ++i << ":"<< d->name <<"    ";
      if (d->description)
          cout << d->description << endl;
      else
          cout<< "(No description available)" << endl;
  }
  int device_number;
  cout << "please select your ethernet device: ";
  cin >> device_number;
  //please set according to your host's ethernert port
  // device_number=5;

  d= alldevs;
  for(i=0; i < device_number -1; i++){
      d=d->next;        
  }
  // cout << ++i << ":"<< d->name <<"    ";
  // if (d->description)
  //     cout << d->description << endl;
  // else
  //     cout<< "(No description available)" << endl;

  /* open the ethernet device*/
  //dev = pcap_lookupdev(err_buf);
  //if (dev==NULL){
  //    cout<<"Could not find device "<<dev<<" : "<<err_buf<<endl;
  //    return -1;
  //}else{
//printf("Found Device %s\n",dev);
  //}
  dev_ = d->name;
  pcapHandle_ = pcap_open_live(dev_,65536,1,10000, err_buf);  //device name, snap length, promiscuous mode, to_ms, error_buffer
  if (pcapHandle_==NULL){
    cout<<"Could not open device "<<dev_<<" : "<<err_buf<<endl;
        exit(1);
  }   

  /* compile the filter */
  if (pcap_compile(pcapHandle_, &fcode, packet_filter, 1, netmask) < 0){
    cout<<"Unable to compile the packet filter. Check the syntax!"<<endl;
    pcap_close(pcapHandle_);
    exit(1);
  }

  /* apply filter*/ 
  if (pcap_setfilter(pcapHandle_, &fcode) < 0){
    cout<<"Filter address error. Can not apply filter!"<<endl;
    pcap_close(pcapHandle_);
    exit(1);
  }
  pcap_freecode(&fcode); 
  // pcap_freecode() is used to free up allocated memory pointed to by a
    // bpf_program struct generated by pcap_compile(3PCAP) when that BPF
    // program is no longer needed, for example after it has been made the
    // filter program for a pcap structure by a call to
    // pcap_setfilter(3PCAP).
    pcap_freealldevs(alldevs);
}

PcapDev::~PcapDev(){
    pcap_close(pcapHandle_);
}


int PcapDev::packetReceiver(){

    int ret=0;
    int fd=0;   //define a file descriptor 
    fd_set rfds; //file descriptor sets for "select" function (it's a bit arrray)
    struct timeval tv;  // strcuture represents elapsed time (declared in sys/time.h)
    const int TIMEOUT=10000;  //timeout in seconds

    fd=pcap_fileno(pcapHandle_); //pcap_fileno returns the descriptor for the packet capture device
    FD_ZERO(&rfds);   //re-clears(empty) file descriptor set 
    FD_SET(fd,&rfds); //rebuild file descriptor set
    
    tv.tv_sec=TIMEOUT;
    tv.tv_usec=0;
 
    ret=select(fd+1, &rfds, NULL, NULL, &tv); 
    //select(): blocks the calling process until there is activity on file descriptor set or until the timeout period has expired
 
    if(-1==ret) cout<<"Select failed"<<endl;
    else if(ret){
        pcap_dispatch(pcapHandle_,1, gotPacket,NULL);
    }else{
        //#ifdef DEBUG
                cout<<"Select timeout on fd:"<<fd<<" Return code: "<<ret<<endl;
        //#endif
    }
    return ret;
}



//Global variable
EthernetCap *p_ecap; 
bool daq_stop=false; 
//Global variable end



void gotPacket(u_char *useless, const struct pcap_pkthdr* pkthdr, 
        const u_char* packet_data){   
    p_ecap->binary_write(pkthdr,packet_data);
    return;
}

EthernetCap::EthernetCap(){
    char filename_time[40];
    time_t sys_time;
    struct tm * timeinfo;
    sys_time = time(0);
    timeinfo = localtime(&sys_time);
    memset(filename_time, 0, sizeof(filename_time)); 
    strftime(filename_time, 40, "../../data/run_%Y%m%d_%H%M%S.dat", timeinfo);
    fp_binary_ = NULL;
    fp_binary_=fopen(filename_time,"wb");
    if(fp_binary_==0){
        printf("Could not open file %s!\n",filename_time);
        exit(1);
    }
    else{
        printf("File %s is opened for binary data storage.\n",filename_time);
    }
    packetNum_ = 0;
    packetNumLast_ = -1;

}

EthernetCap::~EthernetCap(){
    if(fp_binary_!=NULL)
        fclose(fp_binary_);
}

void EthernetCap::binary_write(const struct pcap_pkthdr* pkthdr, 
    const u_char* packet_data){
    int length;
    length=(pkthdr->len-18);  //bytes  preload=14 postload=4
    for(int i=EthernetCap::DATA_START;i<EthernetCap::DATA_START+length;i+=5){
        if(*(packet_data+i)!=EthernetCap::IDLE_WORD){

            fwrite(packet_data+i,1,5,fp_binary_);
        }
    }
    // if(length!=100)
    //   cout<<"length="<<length<<endl;
    if(checkPacketFlag_){
        // int *p = (int*)(packet_data+pkthdr->len-4);
        // packetNum_ = *p;
        // if(packetNum_>65520||packetNum_<20)
        
        packetNum_=(int)(*(packet_data+length+18-2))*256+(int)(*(packet_data+length+18-1));
        // cout<<"packetNum_="<<packetNum_<<"packetNumLast_="<<packetNumLast_<<endl;
        if((packetNum_!=(packetNumLast_+1)%65536)&&(packetNumLast_!=-1)){ 
            printf("warning: %d packets lost! Packet = %d, Last = %d\n", 
                (packetNum_-packetNumLast_+65536)%65536,packetNum_, packetNumLast_);
        }
        packetNumLast_=packetNum_;
    }
    fflush(fp_binary_);
    return;

}

void EthernetCap::setCheckPacketFlagTrue(){
    checkPacketFlag_ = 1;
}


void signalHandler (int signum){
   daq_stop=true;
   cout<<"DAQ stopped!! --"<<signum<<endl;
}


static void show_usage(string name)
{
cerr<< "Usage: " << name << " <option(s)>"
    << "Options:\n"
    << "\t-h,--help\tShow this help message\n"
    // << "\t-n,--name\tSpecify the run name\n"
    << "\t-p,--packet\tHide packet lost\n"     
    << std::endl;
}


int main(int argc, char **argv){
	    /* get run number*/
    cout<<"========================================================== "<<endl;
    int iPacket = 0;   

    //default values for run arguments 
    int show_packet_lost = 1;
    string binary_name = "default";
    // if(argc < 2){
    // 	show_usage(argv[0]);
    // 	return 0;
    // }
    int c;
    int max_packets = -1;
    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"help",    no_argument, 		0,  'h' },                      
            {"packet",  no_argument, 		0,  'p' },
            // {"name",  	required_argument,  0,  'n' }
            {"maxpackets", required_argument, 0 , 'm'}
        };
        c = getopt_long(argc, argv, "hpnm:",long_options, &option_index);
        if (c == -1) break;
        switch (c) {
        	case 'h':
        		show_usage(argv[0]);
        		return 0;
        	case 'p':
        		show_packet_lost = 0;break;
        	// case 'n':
        	// 	runId = optarg;break;
            case 'm':
                max_packets = atoi(optarg);
                std::cout << max_packets << " packets will be recorded" << std::endl;
                break;
        	case '?':
        		cerr<<"unknown or missing arguments!"<<endl;
        		show_usage(argv[0]);
        		return 0;
            default:break;            
        }
    }

    // exit(EXIT_SUCCESS);
    cout<<"run pid is: "<<getpid()<<endl; 


    p_ecap = new EthernetCap();

    //user defined setup for current run
    if(show_packet_lost)
    	p_ecap->setCheckPacketFlagTrue();

    PcapDev *p_Dev = new PcapDev();
    signal(SIGUSR1,signalHandler);

    int i_packet = 0;
    while(p_Dev->packetReceiver()!=0 && daq_stop==false){
        i_packet++;
	if(i_packet%1000 == 0) std::cout << "Recorded "<< i_packet << " packets " << std::endl;
        if(i_packet == max_packets) break;
    }
    delete p_ecap;
    delete p_Dev;
    cout<<"//////Run finished!//////"<<endl;
    cout<< i_packet << " packets recorded " << endl;
    return 0;
}
