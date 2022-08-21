// Troll Purse. All rights reserved.


#include "Core/RpaiState.h"

bool URpaiState::IsEqualTo(const URpaiState* OtherState) const
{
	return false;
}

bool URpaiState::HasReferencedState(const FStateKeyValueReference& StatePropertyReference) const
{
	return !StatePropertyReference.StateKeyName.IsNone() &&
		HasValueWithName(StatePropertyReference.StateKeyName) &&
		StatePropertyReference.ExpectedValueType != EStatePropertyType::Invalid &&
		StatePropertyReference.ExpectedValueType == GetTypeFromName(StatePropertyReference.StateKeyName);
}