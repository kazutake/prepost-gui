#ifndef RIGHTBANKHWM_H
#define RIGHTBANKHWM_H

#include "../points/points.h"

class RightBankHWM : public Points
{
	Q_OBJECT

public:
	RightBankHWM(DataItem* parent);
	~RightBankHWM();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // RIGHTBANKHWM_H
