#include <cmath>
#include <cstdlib>
#include <assert.h>
#include "ann.hpp"

//#define DEBUG 

void ANN::spread(std::vector<double> &data){
	assert(data.size() == inputLayerSize);
	for(int i = 0; i < inputLayerSize; ++i)
		inputLayer[i].output = data[i]; //输入一组学习数据
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

#ifdef DEBUG
	printf("In hiddenLayer:\n");
	for(int i = 0; i < hiddenLayerSize; ++i)
		printf("%.3f ",hiddenLayer[i].output);
	printf("\nIn outputLayer\n");
	for(int i = 0; i < outputLayerSize; ++i)
		printf("%.3f ",outputLayer[i].output);
	printf("\n");
#endif

}

void ANN::bp(std::vector<double> &ans){
	assert(ans.size() == outputLayerSize);
	for(int i = 0; i < outputLayerSize; ++i){
		double op = outputLayer[i].output;
		g[i] = op * (1 - op) * (ans[i] - op); //输出层梯度项
	}
	for(int i = 0; i < hiddenLayerSize; ++i){
		double op = hiddenLayer[i].output ,sigma = 0;
		for(int j = 0; j < outputLayerSize; ++j)
			sigma += g[j] * hiddenLayer[i].w[j];
		e[i] = op * (1 - op) * sigma; //隐藏层梯度项
	}
}

void ANN::bp(std::vector<double> &g,std::vector<double> &e){
	for(int i = 0; i < outputLayerSize; ++i){
		outputLayer[i].threshold -= learningrate * g[i]; //更新输出层阈值
	}
	for(int i = 0; i < hiddenLayerSize; ++i){
		hiddenLayer[i].threshold -= learningrate * e[i]; //更新隐藏层阈值
		for(int j = 0; j < outputLayerSize; ++j)
			hiddenLayer[i].w[j] += learningrate * g[j] * hiddenLayer[i].output; //更新隐藏层权值
	}
	for(int i = 0; i < inputLayerSize; ++i){
		for(int j = 0; j < hiddenLayerSize; ++j)
			inputLayer[i].w[j] += learningrate * e[j] * inputLayer[i].output; //更新输入层权值
	}
}

double ANN::train(std::vector<double> &data,std::vector<double> &ans){
	spread(data);
	bp(ans);
	double fa = 0;
	for(int i = 0; i < outputLayerSize; ++i){
		fa += (outputLayer[i].output - ans[i]) * (outputLayer[i].output - ans[i]);
	}

	for(int i = 0; i < outputLayerSize; ++i)
		sigmaG[i] /= dataCase;
	for(int i = 0; i < hiddenLayerSize; ++i)
		sigmaE[i] /= dataCase;
	//统计偏差量平均值

	bp(sigmaG,sigmaE);

	return delta / dataCase;
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