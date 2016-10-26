void Transpose(vector<char> & buf, const unsigned h, const unsigned w)
{
    for (unsigned start = 0; start <= w * h - 1; ++start)
    {
        unsigned next = start;
        unsigned i = 0;
        do
        {
            ++i;
            next = (next % h) * w + next / h;
        } while (next > start);

        if (next >= start && i != 1)
        {
            const char tmp = buf[start];
            next = start;
            do
            {
                i = (next % h) * w + next / h;
                buf[next] = (i == start) ? tmp : buf[i];
                next = i;
            } while (next > start);
        }
    }
}