#include <algorithm>
#include <iostream>
#include <vector>
#include "Job.hpp"
using namespace std;
/**
 * Problem: Minimize the total penalty for scheduling jobs on a single machine.
 * 
 * Dynamic Programming approach:
 * We define dp[i][t] as the minimum penalty incurred considering the first i jobs 
 * (sorted by deadline) with total elapsed time t.
 * 
 * For each job i, we have two choices:
 * 1. Complete job i on time:
 *    Possible if current time t >= jobs[i].time and t <= jobs[i].deadline.
 *    In this case, penalty = dp[i-1][t - jobs[i].time].
 * 2. Complete job i late:
 *    Penalty = jobs[i].penalty + dp[i-1][t].
 * 
 * We take the minimum of these two choices.
 */

// Global variables to store results (as used in the original code)
int totalPenalty;
vector<int> onTimeJobs;
vector<int> lateJobs;

/**
 * Optimizes job scheduling using Dynamic Programming.
 * Fills the DP table and then backtracks to find the optimal job sequence.
 */
void optimizeJobScheduling(vector<Job>& jobs)
{
    if (jobs.empty()) return;
    
    // Sort jobs by deadline (Earliest Deadline First)
    sort(jobs.begin(), jobs.end());
    
    const size_t size = jobs.size();
    int total_time = 0;
    for(const auto j : jobs) total_time += j.time;

    // dp[i][t] = min penalty for first i jobs at time t
    vector dp(size + 1, vector(total_time + 1, 0));

    // Fill DP table
    for (int i = 1; i <= static_cast<int>(size); ++i) {
        const int p_i = jobs[i-1].penalty;
        const int t_i = jobs[i-1].time;
        const int d_i = jobs[i-1].deadline;
        
        for (int t = 0; t <= total_time; ++t) {
            // Choice 1: Skip job i (or it's late) -> add its penalty
            int skipPenalty = p_i + dp[i-1][t];
            
            // Choice 2: Do job i on time
            int doPenalty = 2e9; // Infinity placeholder
            if (t >= t_i && t <= d_i) {
                doPenalty = dp[i-1][t - t_i];
            }
            
            // Minimum penalty for current state
            dp[i][t] = min(doPenalty, skipPenalty);
            
            // Optimization: if we can achieve a lower penalty with less time, use it
            if (t > 0) dp[i][t] = min(dp[i][t], dp[i][t-1]);
        }
    }

    // Set total penalty from the last cell of the DP table
    totalPenalty = dp[size][total_time];

    // Backtrack to find which jobs were completed on time
    int curTime = total_time;
    
    // Find the minimum time that gives the optimal total penalty
    for (int t = 0; t <= total_time; ++t) {
        if (dp[size][t] == totalPenalty) {
            curTime = t;
            break; 
        }
    }

    for (int i = static_cast<int>(size); i > 0; i--) {
        const int jobTime = jobs[i-1].time;
        const int jobDeadline = jobs[i-1].deadline;
        
        // Adjust curTime if the same penalty was achievable at an earlier time
        while (curTime > 0 && dp[i][curTime] == dp[i][curTime-1]) {
             curTime--;
        }

        // Check if the current job was completed on time in the optimal path
        if (curTime >= jobTime && curTime <= jobDeadline && dp[i][curTime] == dp[i-1][curTime - jobTime]) {
            onTimeJobs.push_back(jobs[i-1].id);
            curTime -= jobTime;
        } else {
            // Job was late
            lateJobs.push_back(jobs[i-1].id);
        }
    }
    
    // Reverse on-time jobs because they were added during backtracking
    reverse(onTimeJobs.begin(), onTimeJobs.end());
}

int main() {
    vector<Job> jobs = {
        {1,18,7,25},
        {2,5,9,20},
        {3,14,6,45},
        {4,9,11,70},
    };
    optimizeJobScheduling(jobs);
    cout << "--- Scheduling Result ---" << endl;
    cout << "Total Penalty: " << totalPenalty << endl;

    cout << "On-Time Sequence: ";
    for(int id : onTimeJobs) cout << id << " ";
    cout << "\nLate Jobs (Penalty paid): ";
    for(int id : lateJobs) cout << id << " ";
    cout << endl;

    cout << "Full Execution Order: ";
    for(const int id : onTimeJobs) cout << id << " ";
    for(const int id : lateJobs) cout << id << " ";
    cout << endl;
}
