// bucket_state.h : 
//

#ifndef __BUCKET_STATE_H__
#define __BUCKET_STATE_H__

const int BUCKETS_COUNT = 3;

typedef struct tagACTION
{
    int from;
    int to;
    int water;
}ACTION;

struct BucketState
{
    BucketState();
    BucketState(const int *buckets);
    BucketState(const BucketState& state);
    BucketState& operator=(const BucketState& state);
    bool IsSameState(const BucketState& state) const;
    bool operator == (const BucketState& state);
    void SetAction(int w, int f, int t);
    void SetBuckets(const int *buckets);
    bool CanTakeDumpAction(int from, int to);
    bool IsBucketEmpty(int bucket);
    bool IsBucketFull(int bucket);
    void PrintStates();
    bool IsFinalState();
    bool DumpWater(int from, int to, BucketState& next);

    int bucket_s[BUCKETS_COUNT];
    ACTION curAction;
};

#endif //__BUCKET_STATE_H__
