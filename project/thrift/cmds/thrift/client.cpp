// Apache Thrift Hello World C++ Client

//Including C++ Headers
#include <iostream>
#include <string>

//Boost libraries
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

//Thrift libraries
#include <thrift/protocol/TBinaryProtocol.h>             
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>                    
#include <thrift/transport/TBufferTransports.h>          
#include <thrift/transport/TTransportUtils.h>

//Including the thrift generated files 
#include "gen-cpp/HelloSvc.h"
#include "gen-cpp/hello_types.h"

//Namespaces
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using boost::make_shared;

//So far pretty much the same as the server 

int main() {
    //Bolier plate code to create the networking connection with the server
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HelloSvcClient client(protocol);
    
    try{
    
    transport->open();
    //------------------------------------------------------------
    
    std::string msg;
    std::cout<<"Initiating contact with server. Gonna wait for the string to return"<<std::endl;
    client.hello_func(msg);
    std::cout << "Received from hello_func: " << msg << std::endl;
    
    //-------------------------------------------------------------------------------- 
    std::cout<<"---------------------------------------------------------"<<std::endl;
    //--------------------------------------------------------------------------------
    
    std::string hello_func2_var1 = " Whoa man, this works, I was set in the client side";
    int64_t hello_fun2_var2 = 112233445566778899;
    client.hello_func2(hello_func2_var1,hello_fun2_var2);
    std::cout<<"Dont need to a return value"<<std::endl;
    //-------------------------------------------------------------------------------
    std::cout<<"---------------------------------------------------------"<<std::endl;
    //-------------------------------------------------------------------------------
    
    std::string hello_func3_var1 = "Whoa man, not again I was set in the client side";
    Hello_struct hello_func3_struct1;
    hello_func3_struct1.num1=123;
    hello_func3_struct1.num1=456;
    hello_func3_struct1.string1="Looky, i am in the struct";
    MyInteger hello_func3_var3 =789;
    client.hello_func3(hello_func3_var1,hello_func3_struct1,hello_func3_var3);

    std::cout<<"DONE"<<std::endl;

    transport->close();
    }catch (TException &tx){
      std::cout<<"Error: " <<tx.what() <<std::endl;
    }
    return 0;
}

