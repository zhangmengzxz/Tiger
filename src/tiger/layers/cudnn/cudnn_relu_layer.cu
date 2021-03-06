#include "tiger/layers/cudnn/cudnn_relu_layer.hpp"
#include "tiger/utils/cudnn.hpp"
namespace tiger{
template <typename Dtype>
void CuDNNReLULayer<Dtype>::forward_gpu(const vector<Blob<Dtype>* >& bottom,
	const vector<Blob<Dtype>* >& top){
    LOG(INFO) << "invoking forward gpu";
    const Dtype* bottom_data = bottom[0]->gpu_data();
    Dtype* top_data = top[0]->mutable_gpu_data();
    CUDNN_CHECK(cudnnActivationForward(this->handle_,
		this->activ_desc_,
		cudnn::dataType<Dtype>::one,
		this->bottom_desc_,
		bottom_data,
		cudnn::dataType<Dtype>::zero,
		this->top_desc_,
		top_data));
}
template <typename Dtype>
void CuDNNReLULayer<Dtype>::backward_gpu(const vector<Blob<Dtype>* >& top,
	const vector<bool>& propagate_down, 
	const vector<Blob<Dtype>* >& bottom){
    LOG(INFO) << "invoking backward gpu ";
    if(!propagate_down[0]){
	return;
    }
    const Dtype* top_data = top[0]->gpu_data();
    const Dtype* top_diff = top[0]->gpu_diff();
    const Dtype* bottom_data = bottom[0]->gpu_data();
    Dtype* bottom_diff = bottom[0]->mutable_gpu_diff();
    CUDNN_CHECK(cudnnActivationBackward(this->handle_,
		this->activ_desc_,
		cudnn::dataType<Dtype>::one,
		this->top_desc_,
		top_data,
		this->top_desc_,
		top_diff,
		this->bottom_desc_,
		bottom_data,
		cudnn::dataType<Dtype>::zero,
		this->bottom_desc_,
		bottom_diff));
}   
template class CuDNNReLULayer<float>;
template class CuDNNReLULayer<double>;
}
