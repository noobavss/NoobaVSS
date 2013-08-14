#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

//opencv includes
//#include <opencv2/core/core.hpp>

#include "NoobaEye.h"

// Qt includes
#include <QObject>

namespace cv{
	class Mat;
};

class FrameProcessor : public QObject
{
	Q_OBJECT

public:

	FrameProcessor(QObject *parent = 0);
	~FrameProcessor();

//	bool procFrame(cv::Mat& in, cv::Mat& out, const nooba::ProcParams& params);

private:
	
};

#endif // FRAMEPROCESSOR_H
