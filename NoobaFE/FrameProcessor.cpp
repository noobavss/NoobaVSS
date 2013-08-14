#include "FrameProcessor.h"

// opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

FrameProcessor::FrameProcessor(QObject *parent)
	: QObject(parent)
{

}

FrameProcessor::~FrameProcessor()
{

}

// bool FrameProcessor::procFrame( cv::Mat& in, cv::Mat& out, const nooba::ProcParams& params)
// {
// 	cv::cvtColor(in, out, CV_BGR2GRAY);
// 	cv::Canny(out,out,100,200);
// 	cv::threshold(out,out,128,255,cv::THRESH_BINARY_INV);
// 	return true;
// }
