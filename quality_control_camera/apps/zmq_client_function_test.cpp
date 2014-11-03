#include <iostream>
#include <zmq.hpp>
#include <Image.pb.h>
#include <Camera.pb.h>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>

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

    delete query;
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

    cv::namedWindow("Received Image", cv::WINDOW_AUTOSIZE);

    zmq::context_t context(1);
    zmq::message_t request;
    uint64_t hwm = 1;

    zmq::socket_t service(context, ZMQ_REQ);
    service.connect("tcp://quality-control-camera:55557");

    // add subscriber to receive status messages from a client
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    subscriber.setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));
    subscriber.connect("tcp://quality-control-camera:55556");

    // give the publisher/subscriber some time to get ready
    sleep(1);

    receiveAndPrintStatusMessage(subscriber);

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

    receiveAndPrintStatusMessage(subscriber);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
