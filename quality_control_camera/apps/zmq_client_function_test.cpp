#include <iostream>
#include <zmq.hpp>
#include <Image.pb.h>
#include <Camera.pb.h>
#include <stdio.h>

void sleep_with_progress(unsigned int seconds)
{
    for (size_t i = 0; i < seconds; ++i)
    {
        std::cout << ". " << std::flush;
        sleep(1);
    }
    std::cout << std::endl;
}

void sendRequest(zmq::socket_t &service, ImageRequest msg)
{
    std::string serialized_string;

    // send message
    msg.SerializeToString(&serialized_string);
    zmq::message_t *query = new zmq::message_t(serialized_string.length());
    memcpy(query->data(), serialized_string.c_str(), serialized_string.length());

    service.send(*query);
}

void receiveAndPrintStatusMessage(zmq::socket_t &subscriber)
{
    zmq::message_t zmq_message;
    CameraStatus status_msg;

    if (subscriber.recv(&zmq_message, ZMQ_NOBLOCK))
    {
        status_msg.ParseFromArray(zmq_message.data(), zmq_message.size());
        std::cout << "is the device connected: " << status_msg.is_device_connected() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    ImageRequest image_request_msg;
    Image image_msg;

    zmq::context_t context(1);
    zmq::message_t request;

    zmq::socket_t service(context, ZMQ_REQ);
    service.connect("tcp://127.0.0.1:55557");   //ToDO change hostname

    // add subscriber to receive status messages from a client
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    subscriber.connect("tcp://127.0.0.1:55556");    //ToDO change hostname

    // give the publisher/subscriber some time to get ready
    sleep(1);

    receiveAndPrintStatusMessage(subscriber);
    receiveAndPrintStatusMessage(subscriber);
    receiveAndPrintStatusMessage(subscriber);
    receiveAndPrintStatusMessage(subscriber);



    std::cout << "Request image" << std::endl;
    image_request_msg = ImageRequest();
    image_request_msg.set_resolution(ImageRequest::WXGA_1280_720);
    sendRequest(service, image_request_msg);

    std::cout << "wait or reply" << std::endl;

    if(service.recv(&request) && image_msg.ParseFromArray(request.data(), request.size()))
        std::cout << "image received -> width: " << image_msg.width() << " height: " << image_msg.height() << std::endl;

    receiveAndPrintStatusMessage(subscriber);


    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}