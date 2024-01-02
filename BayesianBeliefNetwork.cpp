#include <iostream>
#include <vector>
#include <map>
#include<bits/stdc++.h>

using namespace std;

struct Node {
    string name;
    vector<string> parents;
    vector<double> probabilities;
};
map<string, Node> bn = {
    { "B", { "B", {}, { 0.001, 0.999 } } },
    { "J", { "J", { "A" }, { 0.90, 0.10, 0.05, 0.95 } } },
    { "E", { "E", {}, { 0.002, 0.998 } } },
    { "M", { "M", { "A" }, { 0.7, 0.01, 0.3, 0.99} } },
    { "A", { "A", { "B", "E" }, { 0.95, 0.94, 0.29, 0.001, 0.05,0.06, 0.71, 0.999} } }
};
map<string, double> normalize(const map<string, double>& Q) {
    map<string, double> normalizedQ;
    double sum = 0.0;
    for (const auto& entry : Q) {
        sum += entry.second;
    }
    for (const auto& entry : Q) {
        normalizedQ[entry.first] = entry.second / sum;
    }
    return normalizedQ;
}

double query(map<string, bool> evidence)
{
    double p = 1.0;
    for(auto x: evidence)
    {
        if(x.first == "B" || x.first == "E")
        {
            if(x.second == false)
            {
                p*= bn[x.first].probabilities[1];
            }
            else p *= bn[x.first].probabilities[0];
        }
        else if(x.first == "A")
        {
            int sum = 0;
            if(x.second == false)
            {
                sum += 4;
            }
            if(evidence["B"] == false)
            {
                sum += 2;
            }
            if(evidence["E"] == false)
            {
                sum += 1;
            }
            p*= bn[x.first].probabilities[sum];
        }
        else
        {
            int sum = 0;
            if(x.second == false)sum+=2;
            if(evidence["A"] == false)sum+=1;
            p*= bn[x.first].probabilities[sum];
        }
    }
    return p;
}

void generate(vector<map<string, bool>>& allStates, map<string, bool>& state, int cur, vector<string>&var)
{
    if (cur == var.size())
    {
        allStates.push_back(state);
        return;
    }
    string variableName = var[cur];
    state[variableName] = true;
    generate(allStates, state, cur + 1, var);

    state[variableName] = false;
    generate(allStates, state, cur + 1, var);
}

double query(string queryNode, bool val, map<string, bool> evidence)
{
    map<string, Node> hidden;
    vector< string> var;
    double p;
    for(auto x: bn)
    {
        if(evidence.find(x.first) == evidence.end())
        {
            hidden[x.first] = x.second;
            var.push_back(x.first);
        }
    }
    if(hidden.size() == 0)
    {
        return query(evidence);
    }
    vector<map<string, bool>> allStates;
    map<string, bool> currentState;
    generate(allStates, currentState, 0, var);
    double p1 = 0.0;
    double p2 = 0.0;
    for(auto x: allStates)
    {
        map<string, bool> mp = x;
        for(auto y: evidence)
        {
            mp[y.first] = y.second;
        }
        if(mp[queryNode] == true)
        {
            p1 += query(mp);
        }
        else p2 += query(mp);
    }
    if(val == true)
    {
        return (p1/(p1+p2));
    }
    else return (p2/(p1+p2));
}


int main() {
    string queryNode = "B";
    map<string, bool> evidence = { { "J", true }, { "M", true }};
    cout << "P(B = true | (J = true, M = true)) - " << query(queryNode, true, evidence) << endl; 
    evidence = { { "J", false }, { "M", true }};
    cout << "P(B = true | (J = false, M = true)) - " << query(queryNode, true, evidence) << endl; 
    evidence = { { "J", true }, { "M", false }};
    cout << "P(B = true | (J = true, M = false)) - " << query(queryNode, true, evidence) << endl; 
    evidence = { { "J", false }, { "M", false }};
    cout << "P(B = true | (J = false, M = false)) - " << query(queryNode, true, evidence) << endl; 
    evidence = { { "J", true }, { "M", true}, {"A", true}, {"B", false}, {"E", false}};
    cout << "P(B = false, E = false, A = true, J = true, M = true) - " << query(evidence) << endl; 
    return 0;
}
