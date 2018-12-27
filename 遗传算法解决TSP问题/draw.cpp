#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
int main()
{
    FILE *fp=popen("gnuplot","w");
    char *aa="plot for [i=0:198] \"Min.txt\" index i with lines,for [i=0:198] \"average.txt\" index i with lines\n";
    fprintf(fp,"%s",aa);
    fflush(fp);
    sleep(1000);
    return 0;
}
