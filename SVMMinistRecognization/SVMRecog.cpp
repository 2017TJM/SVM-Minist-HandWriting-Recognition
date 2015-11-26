// DigitsRec_HOG_SVM.cpp : �������̨Ӧ�ó������ڵ㡣
#include "opencv2/opencv.hpp"
#include "fstream"
#include "svm.h"
using namespace std;
using namespace cv;

#define srcfeature

vector<string> trainImageList;//ѵ��ͼ���б��˴�·��
vector<int> trainLabelList;   //��ǩ
vector<string> testImageList;//ѵ��ͼ���б��˴�·��
string trainImageFile= "D:\\WorkSpace\\homework\\PatternRecognization\\��һ����ҵ\\minist\\train_image\\imagelist.txt";
string testImageFile = "D:\\WorkSpace\\homework\\PatternRecognization\\��һ����ҵ\\minist\\test_image\\imagelist.txt";
string testBasePath = "D:\\WorkSpace\\homework\\PatternRecognization\\��һ����ҵ\\minist\\test_image\\";
string trainBasePath = "D:\\WorkSpace\\homework\\PatternRecognization\\��һ����ҵ\\minist\\train_image\\";
string SVMModel ="svm_model.xml";
CvMat * dataMat;
CvMat * labelMat;

//***************************************************************
// ����:    readTrainFileList
// ����:    ��ȡѵ����ͼ���б��ͼ���λ��
// Ȩ��:    public 
// ����ֵ:  void
// ����:    string trainImageFile �ļ��б�
// ����:    string basePath ����ַ
// ����:    vector<string> & trainImageList ͼ��·��list
// ����:    vector<int> & trainLabelList ͼ���ǩlist
//***************************************************************
void readTrainFileList(string trainImageFile, string basePath, vector<string> &trainImageList,  vector<int> &trainLabelList)
{
	ifstream readData( trainImageFile );
	string buffer;
	while( readData )
	{    
		if( getline( readData, buffer))    
		{    
			int label = int((buffer[0])-'0');//��������·���е�һ���ļ��о������
			trainLabelList.push_back( label);  
			trainImageList.push_back( buffer );//ͼ��·��       
		}    
	}    
	readData.close();
	cout<<"Read Train Data Complete"<<endl;
}

//***************************************************************
// ����:    readTestFileList
// ����:    �������ļ�
// Ȩ��:    public 
// ����ֵ:  void
// ����:    string testImageFile
// ����:    string basePath
// ����:    vector<string> & testImageList ����ͼ���б�
//***************************************************************
void readTestFileList(string testImageFile, string basePath, vector<string> &testImageList)
{
	ifstream readData( testImageFile );  //���ز���ͼƬ����
	string buffer;
	while( readData )
	{    
		if( getline( readData, buffer))    
		{    
			testImageList.push_back( buffer );//ͼ��·��       
		}    
	}    
	readData.close();
	cout<<"Read Test Data Complete"<<endl;
}

//***************************************************************
// ����:    processHogFeature
// ����:    ����Hog����
// Ȩ��:    public 
// ����ֵ:  void
// ����:    vector<string> trainImageList
// ����:    vector<int> trainLabelList
// ����:    CvMat *  & dataMat
// ����:    CvMat *  & labelMat
//***************************************************************
void processHogFeature(vector<string> trainImageList,vector<int> trainLabelList, CvMat * &dataMat,CvMat * &labelMat)
{
	
	 int trainSampleNum = trainImageList.size();
	 dataMat = cvCreateMat( trainSampleNum, 324, CV_32FC1 );  //324ΪHog feature Size
	 cvSetZero( dataMat );     
	 labelMat = cvCreateMat( trainSampleNum, 1, CV_32FC1 );    
	 cvSetZero( labelMat );    
	 IplImage* src;   
	 IplImage* trainImg=cvCreateImage(cvSize(20,20),8,3);//20 20

	for( int i = 0; i != trainImageList.size(); i++ ) 
	{    
		src=cvLoadImage( (trainBasePath  + trainImageList[i]).c_str(),1);    
		if( src == NULL )    
		{    
			cout<<" can not load the image: "<<(trainBasePath  + trainImageList[i]).c_str()<<endl;    
			continue;    
		}    
		//cout<<"Calculate Hog Feature "<<(trainBasePath  + trainImageList[i]).c_str()<<endl;    

		cvResize(src,trainImg);     
		HOGDescriptor *hog=new HOGDescriptor(cvSize(20,20),cvSize(10,10),cvSize(5,5),cvSize(5,5),9);      
		vector<float>descriptors;    
		hog->compute(trainImg, descriptors,Size(1,1), Size(0,0));     

		int j =0; 
		for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)    
		{    
			cvmSet(dataMat,i,j,*iter);//�洢HOG���� 
			j++;    
		}       
		cvmSet( labelMat, i, 0, trainLabelList[i] );    
		//cout<<"Image and label "<<trainImageList[i].c_str()<<" "<<trainLabelList[i]<<endl;    
	}    
	cout<<"Calculate Hog Feature Complete"<<endl;
	cout<<dataMat<<endl;
}

void processNonFeature(vector<string> trainImageList,vector<int> trainLabelList, CvMat * &dataMat,CvMat * &labelMat)
{

	int trainSampleNum = trainImageList.size();
	dataMat = cvCreateMat( trainSampleNum, 400, CV_32FC1 );  //324ΪHog feature ��С������ǰ���� 
	cvSetZero( dataMat );     
	labelMat = cvCreateMat( trainSampleNum, 1, CV_32FC1 );    
	cvSetZero( labelMat );    
	IplImage* src;   
	IplImage* resizeImg=cvCreateImage(cvSize(20,20),8,3);//20 20��ѵ�������Ĵ�С

	for( int i = 0; i != trainImageList.size(); i++ ) 
	{    
		src=cvLoadImage( (trainBasePath  + trainImageList[i]).c_str(),1);    
		if( src == NULL )    
		{    
			cout<<" can not load the image: "<<(trainBasePath  + trainImageList[i]).c_str()<<endl;    
			continue;    
		}    
		//cout<<"Calculate Hog Feature "<<(trainBasePath  + trainImageList[i]).c_str()<<endl;    

		cvResize(src,resizeImg);     
		IplImage * grayImage = cvCreateImage(cvGetSize(resizeImg), IPL_DEPTH_8U, 1);
		cvCvtColor(resizeImg,grayImage,CV_BGR2GRAY);
		//��ֵ��ͼ��
		IplImage * binaryImage = cvCreateImage(cvGetSize(grayImage),IPL_DEPTH_8U,1);
		cvThreshold(grayImage,binaryImage,25,255,CV_THRESH_BINARY);
		//cvNamedWindow("src");
		//cvShowImage("src", src);

		//cvNamedWindow("show");
		//cvShowImage("show", binaryImage);
		//cvWaitKey(0);//�����ǿ�һ�¶�ֵ����Ч����ô��
		HOGDescriptor *hog=new HOGDescriptor(cvSize(20,20),cvSize(10,10),cvSize(5,5),cvSize(5,5),9);      
		vector<float>descriptors;    


		int j =0; //jΪ�����ˮƽ���꣬Ҫ��������vector�п�������
		uchar * tmp = new uchar;
		for(int n=0;n<binaryImage->height;n++)  
		{	for(int m=0;m<binaryImage->width;m++)  
		{	
			*tmp=((uchar *)(binaryImage->imageData + n*binaryImage->widthStep))[m];  
			cvmSet(dataMat,i,j,*tmp);//�洢HOG���� 
			j++;
		}
		}  

		cvmSet( labelMat, i, 0, trainLabelList[i] );    
		//cout<<"Image and label "<<trainImageList[i].c_str()<<" "<<trainLabelList[i]<<endl;    
	}    
	cout<<"Calculate Hog Feature Complete"<<endl;

}

//***************************************************************
// ����:    trainSVM
// ����:    �˴��õ���opencv��SVMѵ��
// Ȩ��:    public 
// ����ֵ:  void
// ����:    CvMat *  & dataMat
// ����:    CvMat *  & labelMat
//***************************************************************
void trainSVM(CvMat * & dataMat,CvMat * & labelMat )
{
	cout<<"train svm start"<<endl;
	cout<<dataMat<<endl;
	CvSVM svm;
	CvSVMParams param;//������SVMѵ����ز���  
	CvTermCriteria criteria;      
	criteria = cvTermCriteria( CV_TERMCRIT_EPS, 1000, FLT_EPSILON );      
	param = CvSVMParams( CvSVM::C_SVC, CvSVM::RBF, 10.0, 0.09, 1.0, 10.0, 0.5, 1.0, NULL, criteria );          

	svm.train( dataMat, labelMat, NULL, NULL, param );//ѵ������          
	svm.save( SVMModel.c_str());  
	cout<<"SVM Training Complete"<<endl;
}

//***************************************************************
// ����:    trainLibSVM
// ����:    �˴��õ���LibSVM���SVMѵ��
// Ȩ��:    public 
// ����ֵ:  void
// ����:    CvMat * & dataMat
// ����:    CvMat *  & labelMat
//***************************************************************
void trainLibSVM(CvMat *& dataMat, CvMat * & labelMat)
{
	cout<<"LibSVM start"<<endl;
	//����SVM����
	svm_parameter param;
	//param.svm_type = C_SVC;
	param.svm_type = EPSILON_SVR;
	param.kernel_type = RBF;
	param.degree = 10.0;
	param.gamma = 0.09;
	param.coef0 = 1.0;
	param.nu = 0.5;
	param.cache_size = 1000;
	param.C = 10.0;
	param.eps = 1e-3;
	param.p = 1.0;

	//svm_prob��ȡ
	svm_problem svm_prob;
	
	int sampleNum = dataMat->rows;
	int vectorLength = dataMat->cols;

	svm_prob.l = sampleNum;
	svm_prob.y = new double [sampleNum];

	for (int i = 0; i < sampleNum; i++)
	{
		svm_prob.y[i] = cvmGet(labelMat,i,0);
	}

	cout<<"LibSVM middle"<<endl;
	svm_prob.x = new  svm_node * [sampleNum];

	for (int i = 0; i < sampleNum; i++)
	{
		svm_node * x_space = new svm_node [vectorLength + 1];
		for (int j = 0; j < vectorLength; j++)
		{
			x_space[j].index = j;
			x_space[j].value = cvmGet(dataMat,i,j);
			
		}
		x_space[vectorLength].index = -1;//ע�⣬�������ţ�һ��ʼ���Ǽ���

		svm_prob.x[i] = x_space;
	}

	cout<<"LibSVM end"<<endl;
	svm_model * svm_model = svm_train(&svm_prob, &param);
#ifdef srcfeature
	svm_save_model("libsvm_minist_src_feature_model_.model",svm_model);
#else
	svm_save_model("libsvm_minist_model.model",svm_model);
#endif
	for (int i=0 ; i < sampleNum; i++)
	{
		delete [] svm_prob.x[i];
	}

	delete [] svm_prob.y;
	svm_free_model_content(svm_model);
}

//***************************************************************
// ����:    testSVM
// ����:    ����opencvѵ����SVM׼ȷ��
// Ȩ��:    public 
// ����ֵ:  void
// ����:    vector<string> testImageList
// ����:    string SVMModel
//***************************************************************
void testSVM(vector<string> testImageList, string SVMModel)
{
	CvSVM svm;
	svm.load(SVMModel.c_str());//����ģ���ļ�

	IplImage* testImage;  
	IplImage* tempImage;
	char buffer[512]; 

	ofstream ResultOutput( "predict_result.txt" );//��Ԥ�����洢������ı���   
	for( int j = 0; j != testImageList.size(); j++ )//���α������еĴ����ͼƬ    
	{    
		testImage = cvLoadImage( (testBasePath+testImageList[j]).c_str(), 1);    
		if( testImage == NULL )    
		{    
			cout<<" can not load the image: "<<(testBasePath+testImageList[j]).c_str()<<endl;    
			continue;    
		}
		tempImage =cvCreateImage(cvSize(20,20),8,3);
		cvZero(tempImage);    
		cvResize(testImage,tempImage);    
		HOGDescriptor *hog=new HOGDescriptor(cvSize(20,20),cvSize(10,10),cvSize(5,5),cvSize(5,5),9);       
		vector<float>descriptors; 

		hog->compute(tempImage, descriptors,Size(1,1), Size(0,0));       
		CvMat* TempMat=cvCreateMat(1,descriptors.size(),CV_32FC1);    
		int n=0;    
		for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)    
		{    
			cvmSet(TempMat,0,n,*iter);    
			n++;    
		}       

		int resultLabel = svm.predict(TempMat);//�����
		sprintf( buffer, "%s  %d\r\n",testImageList[j].c_str(),resultLabel );
		ResultOutput<<buffer;  
	}
	cvReleaseImage(&testImage);
	cvReleaseImage(&tempImage);
	ResultOutput.close();   
	cout<<"SVM Predict Complete"<<endl;
}

//***************************************************************
// ����:    testLibSVM
// ����:    ����LisbSVMѵ����ģ�͵ķ�������
// Ȩ��:    public 
// ����ֵ:  void
// ����:    string LibSVMModelFile
// ����:    vector<string> testImageList
// ����:    string SVMModel
//***************************************************************
void testLibSVM(string LibSVMModelFile, vector<string> testImageList, string SVMModel)
{

	svm_model * svm = svm_load_model(LibSVMModelFile.c_str());

	IplImage* testImage;  
	IplImage* tempImage;
	char buffer[512]; 

	ofstream ResultOutput( "libsvm_predict_result.txt" ); 
	for( int j = 0; j != testImageList.size(); j++ )//���α������еĴ����ͼƬ    
	{    
		testImage = cvLoadImage( (testBasePath+testImageList[j]).c_str(), 1);    
		if( testImage == NULL )    
		{    
			cout<<" can not load the image: "<<(testBasePath+testImageList[j]).c_str()<<endl;    
			continue;    
		}
		tempImage =cvCreateImage(cvSize(20,20),8,3);
		cvZero(tempImage);    
		cvResize(testImage,tempImage);    
		HOGDescriptor *hog=new HOGDescriptor(cvSize(20,20),cvSize(10,10),cvSize(5,5),cvSize(5,5),9);       
		vector<float>descriptors; 

		hog->compute(tempImage, descriptors,Size(1,1), Size(0,0));  

		svm_node * inputVector = new svm_node [ descriptors.size()+1];
		int n = 0;
		for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)    
		{     
			inputVector[n].index = n;
			inputVector[n].value = *iter;
			n++;
		}       
		inputVector[n].index = -1;
		
		int resultLabel = svm_predict(svm,inputVector);//������
		sprintf( buffer, "%s  %d\r\n",testImageList[j].c_str(),resultLabel );
		ResultOutput<<buffer;  
		delete [] inputVector;
	}
	svm_free_model_content(svm);
	cvReleaseImage(&testImage);
	cvReleaseImage(&tempImage);
	ResultOutput.close();   
	cout<<"SVM Predict Complete"<<endl;
}

//***************************************************************
// ����:    releaseAll
// ����:    �ͷ���Ӧ����Դ
// Ȩ��:    public 
// ����ֵ:  void
//***************************************************************
void releaseAll()
{

	cvReleaseMat( &dataMat ); 
	cvReleaseMat( &labelMat);
	cout<<"Release All Complete"<<endl;
}

//***************************************************************
// ����:    main
// ����:    ������������SVM��һ����opencv�еģ�һ����libsvm�еģ�ѵ��������Ҫѡ�����Ӧ��svm
// Ȩ��:    public 
// ����ֵ:  int
//***************************************************************
int main()
{

	readTrainFileList(trainImageFile,trainBasePath,trainImageList,trainLabelList);      
	processHogFeature(trainImageList,trainLabelList, dataMat,labelMat);
	//trainSVM(dataMat,labelMat );
	//processNonFeature(trainImageList,trainLabelList, dataMat,labelMat);
	trainLibSVM(dataMat,labelMat);
	//readTestFileList( testImageFile,  testBasePath, testImageList);
	testLibSVM("libsvm_minist_model.model",testImageList,SVMModel);
	//testSVM( testImageList, SVMModel);
	releaseAll();
	return 0;
}

