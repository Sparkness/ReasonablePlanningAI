// Troll Purse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Composer/Queries/StateQuery_CompareTo.h"
#include "StateQuery_CompareToInteger.generated.h"

/**
 * Compares the state value as a int to the given int value
 */
UCLASS()
class REASONABLEPLANNINGAI_API UStateQuery_CompareToInteger : public UStateQuery_CompareTo
{
	GENERATED_BODY()
	
public:
	virtual EStateQueryResult Query(const UReasonablePlanningState* StateToQuery) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "RPAI")
	int32 ValueToCompare;

public:
	FORCEINLINE void SetComparisonValue(int32 NewValueToCompare) { ValueToCompare = NewValueToCompare; }
};