// Troll Purse. All rights reserved.


#include "Planners/Planner_AStar.h"
#include "Core/ReasonablePlanningGoalBase.h"
#include "Core/ReasonablePlanningActionBase.h"
#include "Core/ReasonablePlanningState.h"

struct FVisitedState
{
    FGuid Id;
    FGuid ParentId;

    float Cost;
    float Remaining;

    UReasonablePlanningState* State;
    UReasonablePlanningActionBase* Action;
};

bool operator==(const FVisitedState& LHS, const FVisitedState& RHS)
{
    return LHS.Id == RHS.Id;
}

bool operator==(const FVisitedState& LHS, UReasonablePlanningState* RHS)
{
    return LHS.State == RHS;
}

bool operator==(const FVisitedState& LHS, const FGuid& RHS)
{
    return LHS.Id == RHS;
}

bool operator<(const FVisitedState& LHS, const FVisitedState& RHS)
{
    return LHS.Cost + LHS.Remaining < RHS.Cost + RHS.Remaining;
}

UPlanner_AStar::UPlanner_AStar()
    : MaxIterations(1000)
{

}

bool UPlanner_AStar::ReceivePlanChosenGoal_Implementation(const UReasonablePlanningGoalBase* TargetGoal, const UReasonablePlanningState* CurrentState, const TArray<UReasonablePlanningActionBase*>& GivenActions, TArray<UReasonablePlanningActionBase*>& OutActions) const
{
    if (TargetGoal->IsInDesiredState(CurrentState))
    {
        return true; //or false?
    }

    TArray<FVisitedState> OpenActions;
    TArray<FVisitedState> ClosedActions;

    FVisitedState Start;
    Start.Id = FGuid::NewGuid();
    Start.Action = nullptr;
    Start.Cost = 0.f;
    Start.Remaining = 0.f;
    Start.ParentId.Invalidate();
    Start.State = NewObject<UReasonablePlanningState>(GetTransientPackage(), CurrentState->GetClass());

    CurrentState->CopyStateForPredictionTo(Start.State);

    OpenActions.HeapPush(Start);
    int32 Iterations = 0;

    while (OpenActions.Num() > 0 && ++Iterations < MaxIterations)
    {
        FVisitedState Current;
        OpenActions.HeapPop(Current);
        ClosedActions.Push(Current);

        if (TargetGoal->IsInDesiredState(Current.State))
        {
            do
            {
                OutActions.Push(Current.Action);
                auto Next = OpenActions.FindByKey(Current.ParentId);
                if (Next == nullptr)
                {
                    Next = ClosedActions.FindByKey(Current.ParentId);
                }
                Current = *Next;
            } while (Current.ParentId.IsValid());
            return true;
        }

        UReasonablePlanningState* FutureState = NewObject<UReasonablePlanningState>(GetTransientPackage(), CurrentState->GetClass());
        for (const auto& Action : GivenActions)
        {
            if (Action->IsApplicable(Current.State))
            {
                Current.State->CopyStateForPredictionTo(FutureState);
                Action->ApplyToState(FutureState);

                if (ClosedActions.FindByKey(FutureState) != nullptr)
                {
                    continue;
                }

                FVisitedState* InOpen = OpenActions.FindByKey(FutureState);
                auto ActionCost = Action->ExecutionWeight(FutureState);
                auto NewCost = Current.Cost + ActionCost;
                auto NewRemaining = TargetGoal->GetDistanceToDesiredState(FutureState);
                if (InOpen == nullptr)
                {
                    FVisitedState NewNode;
                    NewNode.Id = FGuid::NewGuid();
                    NewNode.Action = Action;
                    NewNode.Cost = NewCost;
                    NewNode.Remaining = NewRemaining;
                    NewNode.ParentId = Current.Id;
                    NewNode.State = NewObject<UReasonablePlanningState>(GetTransientPackage(), CurrentState->GetClass());
                    FutureState->CopyStateForPredictionTo(NewNode.State);

                    OpenActions.HeapPush(NewNode);
                }
                else
                {
                    if (NewCost < InOpen->Cost)
                    {
                        InOpen->ParentId = Current.Id;
                        InOpen->Cost = NewCost;
                        InOpen->Action = Action;
                        InOpen->Remaining = NewRemaining;

                        OpenActions.HeapSort();
                    }
                }
            }
        }
    }

    return false;
}
