#include <cmath>
#include <cstdlib>
#include <assert.h>
#include <string>
#include "ann.hpp"

//#define DEBUG

void ANN::spread(std::vector<double> &data){
	assert(data.size() == inputLayerSize);
	for(int i = 0; i < inputLayerSize; ++i)
		inputLayer[i].output = data[i]; //输入数据
	for(int i = 0; i < hiddenLayerSize; ++i)
		hiddenLayer[i].output = 0;
	for(int i = 0; i < outputLayerSize; ++i)
		outputLayer[i].output = 0;
	//清空当前网络输出初始值
	for(int i = 0; i < hiddenLayerSize; ++i){
		for(int j = 0; j < inputLayerSize; ++j)
			hiddenLayer[i].output += inputLayer[j].output * inputLayer[j].w[i];
		hiddenLayer[i].output = f(hiddenLayer[i].output - hiddenLayer[i].threshold);
	}
	//输入层传播至隐藏层

	for(int i = 0; i < outputLayerSize; ++i){
		for(int j = 0; j < hiddenLayerSize; ++j)
			outputLayer[i].output += hiddenLayer[j].output * hiddenLayer[j].w[i];
		outputLayer[i].output = f(outputLayer[i].output - outputLayer[i].threshold);
	}
	//隐藏层传播至输出层

}

void ANN::bp(std::vector<double> &ans){
	assert(ans.size() == outputLayerSize);
	std::vector<double> g(outputLayerSize),e(hiddenLayerSize);
	for(int i = 0; i < outputLayerSize; ++i){
		double op = outputLayer[i].output;
		g[i] = op * (1 - op) * (ans[i] - op); //输出层梯度项
		outputLayer[i].threshold -= learningrate * g[i]; //更新输出层阈值
	}
	for(int i = 0; i < hiddenLayerSize; ++i){
		double op = hiddenLayer[i].output ,sigma = 0;
		for(int j = 0; j < outputLayerSize; ++j)
			sigma += g[j] * hiddenLayer[i].w[j];
		e[i] = op * (1 - op) * sigma; //隐藏层梯度项
	}
	for(int i = 0; i < hiddenLayerSize; ++i){
		hiddenLayer[i].threshold -= learningrate * e[i]; //更新隐藏层阈值
		for(int j = 0; j < outputLayerSize; ++j)
			hiddenLayer[i].w[j] += learningrate * g[j] * hiddenLayer[i].output; //更新隐藏层权值
	}
	for(int i = 0; i < inputLayerSize; ++i){
		for(int j = 0; j < hiddenLayerSize; ++j)
			inputLayer[i].w[j] += learningrate * e[j] * inputLayer[i].output;
	}
}

double ANN::train(std::vector<double> &data,std::vector<double> &ans){
	spread(data);
	bp(ans);
	double fa = 0;
	for(int i = 0; i < outputLayerSize; ++i){
		fa += (outputLayer[i].output - ans[i]) * (outputLayer[i].output - ans[i]);
	}
	return fa / outputLayerSize;
}

void ANN::setInput(std::vector<double> &data){
	spread(data);
}

void ANN::getOutput(std::vector<double> &ret){
	assert(ret.size() == outputLayerSize);
	for(int i = 0; i < outputLayerSize; ++i){
		ret[i] = outputLayer[i].output;
	}
}

void ANN::outputANN(std::string fileName){
    FILE* fp = fopen(fileName.c_str(),"w");
    fprintf(fp,"%d %d %d\n",inputLayerSize,hiddenLayerSize,outputLayerSize);
    for(int i = 0; i < inputLayerSize; ++i){
        fprintf(fp,"%.10f\n",inputLayer[i].threshold);
        for(int j = 0; j < hiddenLayerSize; ++j)
            fprintf(fp,j?" %.10f":"%.10f",inputLayer[i].w[j]);
        fprintf(fp,"\n");
    }
    for(int i = 0; i < hiddenLayerSize; ++i){
        fprintf(fp,"%.10f\n",hiddenLayer[i].threshold);
        for(int j = 0; j < outputLayerSize; ++j)
            fprintf(fp,j?" %.10f":"%.10f",hiddenLayer[i].w[j]);
        fprintf(fp,"\n");
    }
    for(int i = 0; i < outputLayerSize; ++i){
        fprintf(fp,"%.10f\n",outputLayer[i].threshold);
    }
    fclose(fp);
}
bool ANN::readANN(std::string fileName){
    FILE* fp = fopen(fileName.c_str(),"r");
    if(fp == NULL){
        return false;
    }
    int t1,t2,t3;
    fscanf(fp,"%d%d%d",&t1,&t2,&t3);
    if(inputLayerSize != t1 || hiddenLayerSize != t2 || outputLayerSize != t3){
        fclose(fp);
        return false;
    }
    for(int i = 0; i < inputLayerSize; ++i){
        fscanf(fp,"%lf",&inputLayer[i].threshold);
        for(int j = 0; j < hiddenLayerSize; ++j)
            fscanf(fp,"%lf",&inputLayer[i].w[j]);
    }
    for(int i = 0; i < hiddenLayerSize; ++i){
        fscanf(fp,"%lf\n",&hiddenLayer[i].threshold);
        for(int j = 0; j < outputLayerSize; ++j)
            fscanf(fp,"%lf",&hiddenLayer[i].w[j]);
    }
    for(int i = 0; i < outputLayerSize; ++i){
        fscanf(fp,"%lf",&outputLayer[i].threshold);
    }
    return true;
}
