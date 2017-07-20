#ifndef ESTIMATIONLOGGING_H_
#define ESTIMATIONLOGGING_H_
#include "Lib/PAL/PAL_Lib.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Core/EstimationLogI.h>



namespace Core {
namespace Estimation {

struct EstimationLogElement {
	double x;
	double y;
	NodeId_t id;
	NodeId_t nbr;
	uint64_t time;
	EstimationLogE type;
};


class EstimationLogging: public EstimationLogI {
private:
public:
	~EstimationLogging();
	EstimationLogging();
	void LogEvent(EstimationLogE event, NodeId_t nbr);
};

} //end namespace
}
#endif
