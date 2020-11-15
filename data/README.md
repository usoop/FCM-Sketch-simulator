# Sample Trace

When running the `[Solution].out` file, a sample trace (`data1.dat`) will be downloaded automatically by calling `src/common_func.h`.
This sample trace includes CAIDA-20190117 dataset with 15 second monitoring window. This `data1.dat` file consists of 
- 20 millions of packets
- each packet includes 5-tuple (SrcIp, DstIP, SrcPort, DstPort, and Protocol) which is configured by 13 Bytes. When you read this .dat file, you can use `FILE *fin = fopen(filename, "rb");` and `fread` as implemented in `src/common_func.h`.
- If you configure a flow as its SrcIP (i.e., reading only 4 Bytes out of total 13 Bytes), the average flow size of this trace is about 50, and maximum flow size is near 100,000. The number of flows is nearly 500,000.


Alternatively, the sample trace can also be downloaded by hands from the following link: 
- Google Drive : https://drive.google.com/file/d/1nCRC5-OXn8whYSmG2kZJe-pXu2pierov
After downloading and moving the `data1.dat` file in this directory, common_func.h in src folder will load it. 


For more information of this trace, refer https://www.caida.org/home/.
