#include <sys/stat.h>
#include <fcntl.h>
#include <memory>
#include <fstream>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>


#include "utils/io.hpp"

const int k_proto_read_bytes_limit = INT_MAX;

namespace tiger{

using google::protobuf::io::FileInputStream;
using google::protobuf::io::FileOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::CodedOutputStream;


bool read_proto_from_text_file(const char* filename, Message* proto){
    int fd = open(filename, O_RDONLY);
    CHECK_NE(fd, -1) << "file not found " << filename;
    FileInputStream* input = new FileInputStream(fd);
    bool success = google::protobuf::TextFormat::Parse(input, proto);
    close(fd);
    delete input;
    return success;
}

void write_proto_to_text_file(const Message& proto, const char* filename){
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    FileOutputStream* output = new FileOutputStream(fd);
    CHECK(google::protobuf::TextFormat::Print(proto, output));
    delete output;
    close(fd);
}

bool read_proto_from_binary_file(const char* filename, Message* proto){
    int fd = open(filename, O_RDONLY);
    CHECK_NE(fd, -1) << "file not found " << filename;
    ZeroCopyInputStream* raw_input = new FileInputStream(fd);
    CodedInputStream* coded_input = new CodedInputStream(raw_input);
    coded_input->SetTotalBytesLimit(k_proto_read_bytes_limit, 536870912);
    bool success = proto->ParseFromCodedStream(coded_input);
    close(fd);
    delete raw_input;
    delete coded_input;
    return success;
}

void write_proto_to_binary_file(const Message& proto, const char* filename){
    fstream output(filename, ios::out | ios::trunc | ios::binary);
    CHECK(proto.SerializeToOstream(&output));
}


cv::Mat read_image_to_mat(const string& file_name, const int height,
    const int width, const bool is_color){
    cv::Mat cv_img;
    int cv_read_flag = is_color ? CV_LOAD_IMAGE_COLOR : CV_LOAD_IMAGE_GRAYSCALE;
    cv::Mat cv_img_origin = cv::imread(file_name, cv_read_flag);
    CHECK(cv_img_origin.data) << 
	"load image data failed";
    if(height > 0 && width > 0){
	cv::resize(cv_img_origin, cv_img, cv::Size(width, height));
    }
    else{
	cv_img = cv_img_origin;
    }
    return cv_img;
}

cv::Mat decode_datum_to_mat(const Datum& datum){
    CHECK(datum.encoded()) << "datum not encoded";
    cv::Mat cv_img;
    const string& data = datum.data();
    std::vector<char> vec_data(data.c_str(), data.c_str() + data.size());
    cv_img = cv::imdecode(vec_data, -1);
    CHECK(cv_img.data) << 
	"could not decode datum";
    return cv_img;
}

cv::Mat decode_datum_to_mat(const Datum& datum, bool is_color){
    CHECK(datum.encoded()) << "datum not emcoded";
    
}




cv::Mat transform_datum_to_mat(const Datum& datum){
    CHECK(datum.has_data() && 
	    datum.has_channels() && 
	    datum.has_height() && 
	    datum.has_width()) << 
	"please check datum";
    const char* pixel = datum.data().c_str();
    int channels = datum.channels();
    int height = datum.height();
    int width = datum.width();
    cv::Mat ret_mat(height, width, CV_8UC3);
    CHECK_GT(channels, 0);
    CHECK_LE(channels, 3);
    CHECK_GT(height, 0);
    CHECK_GT(width, 0);
    ret_mat.reshape(height, width);
    int top_index = 0;
    for(int h = 0; h < height; h++){
	for(int w = 0; w < width; w++){
	    for(int c = 0; c < channels; c++){
		top_index = (c * height + h) * width + w;
		ret_mat.at<cv::Vec3b>(h, w)[c] = static_cast<uchar>(pixel[top_index]);
	    }
	}
    } 
    return ret_mat;
}
}


