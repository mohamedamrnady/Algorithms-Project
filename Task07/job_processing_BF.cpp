#include <iostream>
#include <limits>
#include <vector>
#include "Job.hpp"
using namespace std;


/**
 * Problem: A company must schedule jobs on one machine.
 * Each job has a processing time t[i], a deadline d[i], and a penalty p[i] if finished after the deadline.
 * The goal is to minimize the total penalty.
 * 
 * Brute Force approach:
 * Generates all possible permutations of job sequences and calculates the total penalty for each.
 * Time Complexity: O(n.n!), where n is the number of jobs.
 */

// Structure to hold the final optimization result
struct Result {
    std::vector<Job> bestOrder; // The sequence of jobs that results in the minimum penalty
    int totalPenalty = numeric_limits<int>::max(); // The minimum total penalty found
};

/**
 * Calculates the total penalty for a given sequence of jobs.
 * Iterates through the jobs, tracks the completion time, and adds the penalty
 * if a job is finished after its deadline.
 */
static int calculatePenalty(const std::vector<Job> &jobs) {
    int time = 0;
    int totalPenalty = 0;
    for(auto& job : jobs)
    {
        time += job.time;
        if (time > job.deadline) totalPenalty += job.penalty;
    }
    return totalPenalty;
}

/**
 * Recursive helper function to generate all permutations of jobs (Brute Force).
 * @param jobs The original list of jobs.
 * @param current The current permutation being built.
 * @param used Boolean vector to track which jobs are already in the current permutation.
 * @param best Reference to the Result struct to store the best sequence found.
 */
void BFHelper(const std::vector<Job>& jobs,
                             std::vector<Job>& current,
                             std::vector<bool>& used,
                             Result& best){
    // Base case: all jobs are included in the current permutation
    if (current.size() == jobs.size()) {
        if (const int totalPenalty = calculatePenalty(current); totalPenalty < best.totalPenalty)
        {
            best.totalPenalty = totalPenalty;
            best.bestOrder = current;
        }
        return ;
    }

    // Try adding each unused job to the current permutation
    for (size_t i = 0; i < jobs.size(); i++) {
        if (used[i]) continue;
        
        used[i] = true;
        current.push_back(jobs[i]);
        
        BFHelper(jobs, current, used, best);
        
        // Backtrack: remove the job and mark it as unused for other branches
        current.pop_back();
        used[i] = false;
    }
}

/**
 * Main function for the Brute Force optimization.
 * Initializes the recursion to find the best job sequence.
 */
Result optimizeJobScheduling(const vector<Job>& jobs) {
    Result best;
    std::vector<Job> current;
    std::vector<bool> used(jobs.size(), false);
    BFHelper(jobs, current, used, best);
    return best;
}


    int main()
    {
        //----------------------------Brute Force--------------------------------------
        vector<Job> jobs = {
            {1,4,5,10},
            {2,2,6,20},
            {3,1,6,30}
        };
        const auto [bestOrder, totalPenalty] = optimizeJobScheduling(jobs);
        for(const auto job : bestOrder)
        {
            cout << "Job " << job.id << " (Penalty: " << job.penalty << ")\n";

        }
        std::cout << "Total penalty: " << totalPenalty << "\n";
    vector<Job> jobs2 = {
        {1,8,2,17},
        {2,11,1,19},
        {3,3,1,80},
        {4,12,3,51}
    };
    const auto [bestOrder2, totalPenalty2] = optimizeJobScheduling(jobs2);
    for(const auto job : bestOrder2)
    {
        cout << "Job " << job.id << " (Penalty: " << job.penalty << ")\n";

    }
    std::cout << "Total penalty: " << totalPenalty2 << "\n";        return 0;
    }
