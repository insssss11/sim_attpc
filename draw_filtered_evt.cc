#include "draw_filtered_evt/Specification/SpecParticleNum.hh"
#include "draw_filtered_evt/Specification/SpecParticleSecondary.hh"
#include "draw_filtered_evt/Specification/SpecTrue.hh"
#include "draw_filtered_evt/Specification/AndSpecification.hh"
#include "draw_filtered_evt/FilterNext/FilterNext.hh"
#include "draw_filtered_evt/Iterator/EventIterator.hh"
#include "draw_filtered_evt/DrawPadEvent.hh"

#include "TApplication.h"

using namespace std;
using namespace TrainingDataTypes;

using FilterNextEvent = FilterNext<PadEvent>;

int main(int argc, char **argv)
{
    ISpecification<PadEvent> *spec = new SpecTrue<PadEvent>{};
    if(argc < 3)
    {
        printf("Usage : draw_filtered_event <file1> <file2> [-p conf file] [-n particle] [-i particle]\n");
        return 0;
    }
    string file1 = argv[1];
    string file2 = argv[2];
    string file3 = "parameters/config.txt";
    for(int i = 3; i < argc;i += 2)
    {
        if(string(argv[i]) == "-n")
        {
            ISpecification<PadEvent> *excParticle = new SpecParticleSecondary(FromParticleName(argv[i + 1]));
            excParticle->Invert();
            spec = new AndSpecification(*spec, *excParticle);
        }
        else if(string(argv[i]) == "-i")
        {
            ISpecification<PadEvent> *incParticle = new SpecParticleSecondary(FromParticleName(argv[i + 1]));
            spec = new AndSpecification(*spec, *incParticle);            
        }
        else if(string(argv[i]) == "-p")
        {
            file3 = argv[i + 1];
        }            
            
        else if(string(argv[i]) == "--help")
        {
            printf("Usage : draw_filtered_event <file1> <file2> [-p conf file] [-n particle] [-i particle]\n");
            return 0;
        }
        else
        {
            G4cerr << "Invalid argument : " << argv[i] << G4endl;
            return -1;
        }
    }


    if(argc == 4)
        file3 = argv[3];
    auto parTable = ParamContainerTable::GetBuilder()->BuildFromConfigFile(file3);

    DrawPadEvent drawer(parTable);
    EventIterator iter(file1, file2);
    FilterNextEvent filter;

    char key;
    char buffer[256];
    while(true)
    {
        printf("Enter q to escape : ");
        key = getchar();
        if(key == 'q')
            break;
        else if(!filter.NextFiltered(iter, *spec))
        {
            printf("Reached the end of root files.\n");
            break;
        }
        else
        {
            printf("Draing event %8lld ...", iter.GetEventNum());
            snprintf(buffer, 256, "figure%08lld.png", iter.GetEventNum());
            drawer.Draw(filter.GetFiltered(), buffer);
        }
    }
    return -1;
}