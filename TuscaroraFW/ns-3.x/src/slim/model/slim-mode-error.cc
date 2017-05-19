#include "ns3/object.h"
#include "ns3/ptr.h"
#include "slim-mode-error.h"

namespace ns3 {
TypeId SlimModeError::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Slim-Mode-Error")
    .SetParent<Object> ()
	;
  return tid;
}
}

