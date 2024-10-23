#include "neural_network.h"
#include "table_select.h"
int main() 
{

    int selectedmode = 0;
    uiTable mainTable;
    mainTable.configTable({ "Simple Addition Agent", "Binary Memory Demo" });
    while (selectedmode == 0) 
    {
        system("cls");
        cout << "Use arrow keys to select mode (enter to continue): \n\n";
        mainTable.print();
        cout << endl;
        if (mainTable.testInput(72, 80, true) == 13) 
        {
            selectedmode = mainTable.index + 1;
        }
    }
    system("cls");

    //define your neural network
    nn exampleAgent; 
    exampleAgent.configlayer(0, 4);  //first layer
    exampleAgent.configlayer(1, 9);  //hidden layer
    exampleAgent.configlayer(2, 4);  //output layer
    exampleAgent.gClipping = false;
    exampleAgent.learningRate = 0.02;
    exampleAgent.linearActivation = true;
    
    //write the flashcards for the network to study
    trainingSet exampleSet;
    if (selectedmode == 1) 
    {
        exampleAgent.configlayer(0, 2);
        exampleAgent.configlayer(exampleAgent.neuronValues.size() - 1, 1);
        exampleSet.flashcard({ 0,1 }, { 1 });
        exampleSet.flashcard({ 1,0 }, { 1 });
        exampleSet.flashcard({ 0,0 }, { 0 });
        cout << "training the network to add 2 numbers together\n";
    }
    else 
    {
        exampleAgent.configlayer(0, 4);
        exampleAgent.configlayer(exampleAgent.neuronValues.size() - 1, 4);
        exampleSet.flashcard({ 0,1,1,0 }, { 0,1,0,1 });
        exampleSet.flashcard({ 1,0,0,0 }, { 1,1,0,1 });
        exampleSet.flashcard({ 0,1,0,0 }, { 0,1,1,1 });
        exampleSet.flashcard({ 0,0,1,0 }, { 0,1,0,1 });
        exampleSet.flashcard({ 1,0,1,0 }, { 0,1,0,0 });
        exampleSet.flashcard({ 1,0,1,1 }, { 0,0,1,0 });
        cout << "Training the network to memorise a random set of binary definitions\n";
    }

    for (int num = 0; num < exampleAgent.neuronValues.size(); num++) 
    {
        cout << "Layer " << num + 1 << " Size: " << exampleAgent.neuronValues[num].size() << " neurons" << endl;
    }

    //simple loading screen (true itoration count is 100,000)
    for (int i = 0; i < 10; i++) 
    {
        exampleSet.study(exampleAgent, 10'000);
        cout << 10 - i << ".. ";
    }
    cout << endl;

    //finding acurate score 
    float fitness = exampleSet.study(exampleAgent, 10);
 
    cout << fixed << setprecision(6) << "Network fitness: " << fitness << " % \n\n" << "Set the inputs of the neurons to test its output:\n";
    while (true) 
    {
        vector<float> inputter;
        for (int i = 0; i < exampleAgent.neuronValues[0].size(); i++) 
        {
            float response;
            cout << "\nNeuron " << i + 1 << ": ";
            cin >> response;
            inputter.push_back(response);
        }

        //Plugs in the user responce inputs into the network and assigns its outputs to a vector
        vector<float> outputs = exampleAgent.run(inputter);



        cout << "\nPrediction: [ ";
        for (int i = 0; i < outputs.size(); i++) 
        {
            string iscomma = (i == outputs.size() - 1) ? " " : ", ";
            cout << setprecision(3) << abs((outputs[i])) << iscomma;
        }
        cout << "]\n";
    }
}
