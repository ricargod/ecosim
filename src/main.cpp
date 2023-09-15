#define CROW_MAIN
#define CROW_STATIC_DIR "../public"

#include "crow_all.h"
#include "json.hpp"
#include <iostream>
#include <random>

static const uint32_t NUM_ROWS = 15;

// Constants
const uint32_t PLANT_MAXIMUM_AGE = 10;
const uint32_t HERBIVORE_MAXIMUM_AGE = 50;
const uint32_t CARNIVORE_MAXIMUM_AGE = 80;
const uint32_t MAXIMUM_ENERGY = 200;
const uint32_t THRESHOLD_ENERGY_FOR_REPRODUCTION = 20;

// Probabilities
const double PLANT_REPRODUCTION_PROBABILITY = 0.2;
const double HERBIVORE_REPRODUCTION_PROBABILITY = 0.075;
const double CARNIVORE_REPRODUCTION_PROBABILITY = 0.025;
const double HERBIVORE_MOVE_PROBABILITY = 0.7;
const double HERBIVORE_EAT_PROBABILITY = 0.9;
const double CARNIVORE_MOVE_PROBABILITY = 0.5;
const double CARNIVORE_EAT_PROBABILITY = 1.0;

// Type definitions
enum entity_type_t
{
    empty,
    plant,
    herbivore,
    carnivore
};

struct pos_t
{
    uint32_t i;
    uint32_t j;
};

struct entity_t
{
    entity_type_t type;
    int32_t energy;
    int32_t age;
};

static std::random_device rd;
static std::mt19937 gen(rd());
//FUNÇÕES
// Function to generate a random action based on probability
bool random_action(float probability) {
    //static std::random_device rd;
    //static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < probability;
}
// Auxiliary code to convert the entity_type_t enum to a string
NLOHMANN_JSON_SERIALIZE_ENUM(entity_type_t, {
                                                {empty, " "},
                                                {plant, "P"},
                                                {herbivore, "H"},
                                                {carnivore, "C"},
                                            })

// Auxiliary code to convert the entity_t struct to a JSON object
namespace nlohmann
{
    void to_json(nlohmann::json &j, const entity_t &e)
    {
        j = nlohmann::json{{"type", e.type}, {"energy", e.energy}, {"age", e.age}};
    }
}

// Grid that contains the entities
static std::vector<std::vector<entity_t>> entity_grid;
std::vector<std::pair<int, int>> pares_analisados;
std::vector<std::pair<int, int>> posicoes_disponiveis;
int main()
{
    crow::SimpleApp app;

    // Endpoint to serve the HTML page
    CROW_ROUTE(app, "/")
    ([](crow::request &, crow::response &res)
     {
        // Return the HTML content here
        res.set_static_file_info_unsafe("../public/index.html");
        res.end(); });

    CROW_ROUTE(app, "/start-simulation")
        .methods("POST"_method)([](crow::request &req, crow::response &res)
                                { 
        // Parse the JSON request body
        nlohmann::json request_body = nlohmann::json::parse(req.body);

       // Validate the request body 
        uint32_t total_entinties = (uint32_t)request_body["plants"] + (uint32_t)request_body["herbivores"] + (uint32_t)request_body["carnivores"];
        if (total_entinties > NUM_ROWS * NUM_ROWS) {
        res.code = 400;
        res.body = "Too many entities";
        res.end();
        return;
        }

        // Clear the entity grid
        entity_grid.clear();
        entity_grid.assign(NUM_ROWS, std::vector<entity_t>(NUM_ROWS, { empty, 0, 0}));
        std::uniform_int_distribution<> dis(0, 14);
        int linha = dis(gen);
        int coluna = dis(gen);
        // Create the entities
        for(uint32_t i=0;i<(uint32_t)request_body["plants"];i++){
            //cria as planta
            while (!entity_grid[linha][coluna].type==empty){
                linha = dis(gen);
                coluna = dis(gen); 
            }
            entity_grid[linha][coluna].type=plant;
            entity_grid[linha][coluna].age=0;
        }
        for(uint32_t i=0;i<(uint32_t)request_body["herbivores"];i++){
            //cria os coelho
            while (!entity_grid[linha][coluna].type==empty){
                linha = dis(gen);
                coluna = dis(gen); 
            }
            entity_grid[linha][coluna].type=herbivore;
            entity_grid[linha][coluna].age=0;

        }
        for(uint32_t i=0;i<(uint32_t)request_body["carnivores"];i++){
            //cria os leao
            while (!entity_grid[linha][coluna].type==empty){
                linha = dis(gen);
                coluna = dis(gen); 
            }
            entity_grid[linha][coluna].type=carnivore;
            entity_grid[linha][coluna].age=0;
        }
        // <YOUR CODE HERE>

        // Return the JSON representation of the entity grid
        nlohmann::json json_grid = entity_grid; 
        res.body = json_grid.dump();
        res.end(); });

    // Endpoint to process HTTP GET requests for the next simulation iteration
    CROW_ROUTE(app, "/next-iteration")
        .methods("GET"_method)([]()
                               {
        // Simulate the next iteration
        // Iterate over the entity grid and simulate the behaviour of each entity
        
        // <YOUR CODE HERE>
        //Analisa todas as casas
        
        bool analisada=false;
        for(uint32_t i=0;i<NUM_ROWS;i++){
            for(uint32_t j=0;j<NUM_ROWS;j++){
                //verifica se a casa ja foi analisada
                analisada=false;
                for (int k=0;k<pares_analisados.size();k++){
                    if (pares_analisados[k].first==i && pares_analisados[k].second==j){
                        analisada=true;
                    }
                }
                
                //caso nao tenha sido analisada
                if (!analisada){
                    //SE FOR PLANTA
                    if (entity_grid[i][j].type==plant){
                        //se for velha morre
                        if (entity_grid[i][j].age==10){
                            entity_grid[i][j].type=empty;
                            entity_grid[i][j].energy=0;
                            entity_grid[i][j].age=0;
                        }else{
                            entity_grid[i][j].age++;
                            if (random_action(PLANT_REPRODUCTION_PROBABILITY)) {
                                //analisa casas adjacentes e coloca as disponiveis em um vetor
                                std::cout << "Test.\n";
                                if((i+1)<NUM_ROWS){
                                    if (entity_grid[i+1][j].type==empty){
                                        posicoes_disponiveis.push_back(std::make_pair(i+1, j));
                                        std::cout << "Test1.\n"<<i<<"\n"<<j<<"\n";
                                    }
                                }
                                
                                std::cout << "Test2.\n"<<i<<"\n"<<j<<"\n";
                                if (i>0){
                                    if (entity_grid[i-1][j].type==empty){
                                        posicoes_disponiveis.push_back(std::make_pair(i-1, j));
                                        std::cout << "Test3.\n"<<i<<"\n"<<j<<"\n";
                                    }
                                }else{
                                    std::cout<<"Extra";
                                }
                                
                                std::cout << "Test4.\n"<<i<<"\n"<<j<<"\n";
                                if ((j+1)<NUM_ROWS){
                                    if (entity_grid[i][j+1].type==empty){
                                        posicoes_disponiveis.push_back(std::make_pair(i, j+1));
                                        std::cout << "Test5.\n"<<i<<"\n"<<j<<"\n";
                                    }
                                }
                                std::cout << "Test6.\n"<<i<<"\n"<<j<<"\n";
                                if(j>0){
                                    if (entity_grid[i][j-1].type==empty){
                                        posicoes_disponiveis.push_back(std::make_pair(i, j-1));
                                        std::cout << "Test7.\n"<<i<<"\n"<<j<<"\n";
                                    }
                                }else{
                                    std::cout<<"Extra";
                                }
                                std::cout << "Test8.\n"<<i<<"\n"<<j<<"\n";
                                if (!posicoes_disponiveis.empty()){
                                    std::uniform_int_distribution<> dis(0, posicoes_disponiveis.size()-1);
                                    int sorteio = dis(gen);
                                    int x=posicoes_disponiveis[sorteio].first;
                                    int y=posicoes_disponiveis[sorteio].second;
                                    std::cout<<"sorteio"<<sorteio<<"\n"<<x<<"\n"<<y<<"\n";
                                    entity_grid[x][y].type = plant;
                                    pares_analisados.push_back(std::make_pair(x, y));
                                    posicoes_disponiveis.clear();
                                }
                            } 
                        }
                    }
                }
            }
        }
        pares_analisados.clear();
        // Return the JSON representation of the entity grid
        nlohmann::json json_grid = entity_grid; 
        return json_grid.dump(); });
    app.port(8080).run();

    return 0;
}