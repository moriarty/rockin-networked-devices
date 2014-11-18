#include <iostream>
#include <zmq.hpp>
#include <DeviceImage.pb.h>
#include <DeviceCamera.pb.h>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <unistd.h>

void sendRequest(zmq::socket_t &service, ImageRequest msg)
{
    std::string serialized_string;

    // send message
    msg.SerializeToString(&serialized_string);
    zmq::message_t *query = new zmq::message_t(serialized_string.length());
    memcpy(query->data(), serialized_string.c_str(), serialized_string.length());

    service.send(*query);

    delete query;
}

int main(int argc, char *argv[])
{
    ImageRequest image_request_msg;
    Image image_msg;

    cv::namedWindow("Received Image", cv::WINDOW_AUTOSIZE);

    zmq::context_t context(1);
    zmq::message_t request;
    uint64_t hwm = 1;

    zmq::socket_t service(context, ZMQ_REQ);
    service.connect("tcp://192.168.1.102:55522");

    // give the publisher/subscriber some time to get ready
    sleep(1);

    for (unsigned int i = 0; i < 20; i++)
    {
        image_request_msg = ImageRequest();
        sendRequest(service, image_request_msg);

        if (service.recv(&request) && image_msg.ParseFromArray(request.data(), request.size()))
        {
            cv::Mat image_to_display(image_msg.height(), image_msg.width(), image_msg.encoding(), const_cast<char*>(&image_msg.data()[0]), image_msg.step());

            cv::imshow("Received Image", image_to_display);
            cv::waitKey(100);
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
