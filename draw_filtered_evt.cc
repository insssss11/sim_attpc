#include "draw_filtered_evt/Specification/SpecParticleNum.hh"
#include "draw_filtered_evt/Specification/SpecParticleSecondary.hh"
#include "draw_filtered_evt/FilterNext/FilterNext.hh"
#include "draw_filtered_evt/Iterator/EventIterator.hh"
#include "draw_filtered_evt/DrawPadEvent.hh"

#include "TApplication.h"

using namespace std;
using namespace TrainingDataTypes;

using FilterNextEvent = FilterNext<PadEvent>;

int main(int argc, char **argv)
{
    string file1 = argv[1];
    string file2 = argv[2];
    string file3 = "parameters/config.txt";
    if(argc == 4)
        file3 = argv[3];
    auto parTable = ParamContainerTable::GetBuilder()->BuildFromConfigFile(file3);

    DrawPadEvent drawer(parTable);
    EventIterator iter(file1, file2);
    FilterNextEvent filter;
    SpecParticleNum specNum(0);
    SpecParticleSecondary specType(Carbon);
    char key;
    while(true)
    {
        printf("Enter q to escape : ");
        key = getchar();
        if(key == 'q')
            break;
        else if(!filter.NextFiltered(iter, specType))
        {
            printf("Reached the end of root files.\n");
            break;
        }
        else
            drawer.Draw(filter.GetFiltered());
    }
    return -1;
}