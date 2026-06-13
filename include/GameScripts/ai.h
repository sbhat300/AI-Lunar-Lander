#ifndef AI_H
#define AI_H

#include <Engine/engine.h>
#include <Engine/sharedData.h>
#include <GameScripts/TextureManager/textureManager.h>

#include <EngineIncludes/entity.h>
#include <NEAT/neat.h>
#include <sstream>
#include <GameScripts/goal.h>
#include <GameScripts/aiLander.h>

class aiScript : public baseScript
{
    public:
        NEAT population;
        int populationSize = 300;
        std::vector<entity*> landers;
        entity* goal;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> distr;
        int iteration = 0;

        bool evaluating = false;
        bool renderAll = false;

        std::string serializeNetwork(const NEAT::genome& g) 
        {
            std::ostringstream oss;

            oss << "python3 src/visualize.py \"";
            
            oss << g.nnExecutionOrder.size() << " " << g.inputs << " " << g.outputs << " ";
            for (const auto& neuron : g.nnExecutionOrder) 
            {
                oss << neuron.bias << " " << neuron.isOutput << " " << neuron.id << " " << neuron.outgoing.size();
                for (const auto& conn : neuron.outgoing) 
                {
                    oss << " " << conn.targetNodeID << " " << conn.weight << " ";
                }
            }
            oss << " \"\n";
            return oss.str();
        }

        void initialize()
        {
            population.initialize();
            population.generatePopulation(populationSize, 10, 2);
            for(int i = 0; i < populationSize; i++) population.compileNetwork(population.genomes[i]);
        }

        void evaluate()
        {
            goal = new entity("goal", glm::vec2(0, 0), glm::vec2(40, 50), 0);
            goal->addScript<goalScript>(); 

            landers.resize(population.populationSize);
            std::unordered_set<int> uniqueSpecies;
            for(int i = 0; i < population.populationSize; i++)
            {
                landers[i] = new entity("aiLander", glm::vec2(distr(gen), 300), glm::vec2(50, 50), 0);
                landers[i]->addScript<aiLanderScript>();
                ((aiLanderScript*)landers[i]->scripts[0])->goal = (goalScript*)goal->scripts[0];
                aiLanderScript* landerScript = (aiLanderScript*)landers[i]->scripts[0];
                if(!renderAll) landers[i]->polygonInstance.shouldRender = population.genomes[i].isChampion;
                landerScript->genome = &population.genomes[i];
                landerScript->population = &population;    
                landerScript->speciesId = population.genomes[i].speciesId;
                uniqueSpecies.insert(population.genomes[i].speciesId);
                landerScript->preUpdate();
            }
            std::cout << "NUMBER OF SPECIES: " << uniqueSpecies.size() << std::endl;
        }

        void reproduce()
        {
            delete goal;    
            for(int i = 0; i < population.populationSize; i++) delete landers[i];
            landers.clear();
            population.speciate();
            population.fitnessShare();
            population.reproduce();
            for(int i = 0; i < population.populationSize; i++) population.compileNetwork(population.genomes[i]);
        }

        void start() override
        {
            gen = std::mt19937(rd());
            distr = std::uniform_real_distribution<float>(-340, 340);
            population.minNGenes = 30;
            initialize();
        }

        void update() override
        {
            if(evaluating)
            {
                bool aliveLander = false;
                float bestScore = FLT_MIN, worstScore = FLT_MAX;
                int bestId = 0;
                std::unordered_map<int, int> speciesRenderCount;
                for(int i = 0; i < population.populationSize; i++)
                {
                    aiLanderScript* landerScript = (aiLanderScript*)landers[i]->scripts[0];
                    if(landerScript->score >= bestScore)
                    {
                        bestScore = landerScript->score;
                        bestId = i;
                    }
                    worstScore = std::min(worstScore, landerScript->score);
                    if(!landerScript->dead) aliveLander = true;
                    if(!renderAll) 
                    {
                        int sID = population.genomes[i].speciesId; 
                        if(!population.genomes[i].isChampion)
                        {
                            if (speciesRenderCount[sID] < 2) 
                            {
                                landers[i]->polygonInstance.shouldRender = true;
                                speciesRenderCount[sID]++; 
                            } 
                            else 
                            {
                                landers[i]->polygonInstance.shouldRender = false;
                            }
                        }
                    }
                }
                if(!aliveLander)
                {
                    std::cout << "ITERATION " << ++iteration << ", BEST SCORE: " << bestScore << std::endl;
                    evaluating = false;
                    std::cout << serializeNetwork(population.genomes[bestId]) << std::endl;
                    if(worstScore < 0)
                    {
                        for(int i = 0; i < populationSize; i++) 
                        {
                            population.genomes[i].fitness += -worstScore;
                        }
                    }
                    reproduce();
                }
            }
            else if(landers.size() == 0)
            {
                evaluate();
                evaluating = true;
            }
        }
};

#endif