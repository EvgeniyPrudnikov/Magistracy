#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>
#include <iterator>


void simpleSample(int n, int m, int rep, std::string filename)
{
    std::vector<int> hist_v(n, 0);

    srand((unsigned int) time(NULL));

    std::vector<int> stream(n);
    for (int j = 0; j < n; ++j)
    {
        stream[j] = j;
    }

    std::random_shuffle(stream.begin(), stream.end());

    for (int l = 0; l < rep; ++l)
    {
        std::vector<int> sample;

        for (int i = 0; i < n; ++i)
        {
            if (i < m)
            {
                sample.push_back(stream[i]);
            } else
            {
                double probInsert = (double) m / (i + 1);
                double r = (double) rand() / RAND_MAX;

                if (r <= probInsert)
                {
                    int del_idx = rand() % m;
                    sample.erase(sample.begin() + del_idx);
                    sample.push_back(stream[i]);
                }
            }
        }

        for (int k = 0; k < m; ++k)
        {
            hist_v[sample[k]]++;
        }
    }

    std::ofstream f(filename, std::ios::out);
    std::copy(hist_v.begin(), hist_v.end(), std::ostream_iterator<int>(f, " "));
    f.close();
}


struct weight_elem
{
    int elem;
    int weight;
};

struct KeyValue
{
    double key;
    int value;
};

struct compare
{
    bool operator()(const KeyValue &l, const KeyValue &r)
    {
        return l.key > r.key;
    }
};

void weightSample(int n, int m, int rep, int w_pow, std::string filename)
{
    std::vector<int> hist_v(n, 0);

    srand((unsigned int) time(NULL));

    std::vector<weight_elem> stream(n);
    for (int j = 0; j < n; ++j)
    {
        stream[j] = {j, (int) pow(j, w_pow)};
    }

    std::random_shuffle(stream.begin(), stream.end());

    for (int l = 0; l < rep; ++l)
    {
        std::priority_queue<KeyValue, std::vector<KeyValue>, compare> sample;

        for (int i = 0; i < n; ++i)
        {
            double r_i = pow(((double) rand() / RAND_MAX), (1 / (double) stream[i].weight));

            if (i < m)
            {
                sample.push({r_i, stream[i].elem});

            } else
            {
                if (sample.top().key < r_i)
                {
                    sample.pop();
                    sample.push({r_i, stream[i].elem});
                }
            }
        }

        for (int k = 0; k < m; ++k)
        {
            hist_v[sample.top().value]++;
            sample.pop();
        }
    }

    std::ofstream f(filename, std::ios::out);
    std::copy(hist_v.begin(), hist_v.end(), std::ostream_iterator<int>(f, " "));
    f.close();
}


int main(int argc, char *argv[])
{

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int rep = atoi(argv[3]);

    simpleSample(n, m, rep, "hist.txt");
    weightSample(n, m, rep, 1, "hist_weight_i1.txt");
    weightSample(n, m, rep, 2, "hist_weight_i2.txt");

    return 0;
}