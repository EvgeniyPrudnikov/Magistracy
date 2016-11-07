
void mergeRuns(ifstream &infile1, ifstream &infile2, ofstream &outfile, long run1_start, long run2_start, long run2_end)
{
    long blk_size_B1 = block_size_B;
    long blk_size_B2 = block_size_B;

    long read_size_B1 = block_size_B * sizeof(long);
    long read_size_B2 = block_size_B * sizeof(long);

    long read_blk1_offset = run1_start;
    long read_blk2_offset = run2_start;

    if (run2_end - read_blk2_offset < read_size_B2)
    {
        blk_size_B2 = (run2_end - read_blk2_offset) / sizeof(long);
        read_size_B2 = (run2_end - read_blk2_offset);
    }


    vector<long> bufferBr1(blk_size_B1);
    vector<long> bufferBr2(blk_size_B2);

    infile1.seekg(run1_start, ios::beg);
    infile1.read((char *) &bufferBr1[0], read_size_B1);

    infile2.seekg(run2_start, ios::beg);
    infile2.read((char *) &bufferBr2[0], read_size_B2);

    vector<long> bufferBw(block_size_B);

    int p1 = 0, p2 = 0, po = 0;

    while (read_blk1_offset < run2_start && read_blk2_offset < run2_end)
    {
        if (run2_end - read_blk2_offset < read_size_B2)
        {
            blk_size_B2 = (run2_end - read_blk2_offset) / sizeof(long);
            read_size_B2 = (run2_end - read_blk2_offset);
        }

        if (bufferBr1[p1] < bufferBr2[p2])
        {
            bufferBw[po] = bufferBr1[p1];
            p1++;

            if (p1 > blk_size_B1 - 1)
            {

                read_blk1_offset += read_size_B1;

                if (read_blk1_offset < run2_start)
                {
                    infile1.read((char *) &bufferBr1[0], read_size_B1);
                    p1 = 0;
                }
            }
        } else
        {
            bufferBw[po] = bufferBr2[p2];
            p2++;
            if (p2 > blk_size_B2 - 1)
            {
                read_blk2_offset += read_size_B2;

                if (read_blk2_offset < run2_end)
                {
                    infile2.read((char *) &bufferBr2[0], read_size_B2);
                    p2 = 0;
                }
            }
        }

        po++;
        if (po > blk_size_B1 - 1)
        {
            outfile.write((char *) &bufferBw[0], read_size_B1);
            po = 0;
        }
    }

    read_size_B1 = block_size_B * sizeof(long);
    read_size_B2 = block_size_B * sizeof(long);

    if (run2_end - read_blk2_offset < read_size_B2)
    {
        blk_size_B2 = (run2_end - read_blk2_offset) / sizeof(long);
        read_size_B2 = (run2_end - read_blk2_offset);
    }

    if (read_blk1_offset == run2_start)
    {
        for (int j = p2; j < blk_size_B2; ++j)
        {
            bufferBw[po] = bufferBr2[j];
            po++;
        }
        outfile.write((char *) &bufferBw[0], po * sizeof(long));
        read_blk2_offset += po * sizeof(long);


        for (long i = read_blk2_offset; i < run2_end; i += read_size_B2)
        {
            if (run2_end - read_blk2_offset < read_size_B2)
            {
                read_size_B2 = (run2_end - read_blk2_offset);
            }

            infile2.read((char *) &bufferBr2[0], read_size_B2);
            outfile.write((char *) &bufferBr2[0], read_size_B2);
        }
    } else if (read_blk2_offset == run2_end)
    {

        for (int j = p1; j < blk_size_B1; ++j)
        {
            bufferBw[po] = bufferBr1[j];
            po++;
        }
        outfile.write((char *) &bufferBw[0], po * sizeof(long));
        read_blk1_offset += po * sizeof(long);


        for (long i = read_blk1_offset; i < run2_start; i += read_size_B1)
        {
            if (run2_start - read_blk1_offset < read_size_B1)
            {
                read_size_B1 = (run2_start - read_blk1_offset);
            }

            infile1.read((char *) &bufferBr1[0], read_size_B1);
            outfile.write((char *) &bufferBr1[0], read_size_B1);
        }
    }

    bufferBr1.clear();
    bufferBr2.clear();
    bufferBw.clear();

}