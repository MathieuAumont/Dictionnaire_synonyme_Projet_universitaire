#include <iostream>
#include <algorithm>
#include <string>

float similitude(const std::string& mot1, const std::string& mot2)
{
    //algorithme de similitude trouver sur https://www.techiedelight.com/find-similarity-between-two-strings-in-cpp/
    //Edit distance and Jaro-Winkler
    //modifier pour le bien de cas présent
    int m = mot1.length();
    int n = mot2.length();

    int T[m + 1][n + 1];
    for (int i = 1; i <= m; i++) {
        T[i][0] = i;
    }

    for (int j = 1; j <= n; j++) {
        T[0][j] = j;
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int weight = mot1[i - 1] == mot2[j - 1] ? 0: 1;
            T[i][j] = std::min(std::min(T[i-1][j] + 1, T[i][j-1] + 1), T[i-1][j-1] + weight);
        }
    }

    return T[m][n];
}//
// Created by mathi on 2024-07-16.
//
