#include <alibabacloud/oss/OssClient.h>

class BucketSample {
public:
  BucketSample(const std::string &bucket);
  ~BucketSample();

  void DoesBucketExist();

  void InvalidBucketName();
  void CreateAndDeleteBucket();
  void SetBucketAcl();
  void SetBucketLogging();
  void SetBucketWebsite();
  void SetBucketReferer();
  void SetBucketLifecycle();
  void SetBucketCors();

  void DeleteBucketLogging();
  void DeleteBucketWebsite();
  void DeleteBucketLifecycle();
  void DeleteBucketCors();

  void GetBucketAcl();
  void GetBucketLocation();
  void GetBucketLogging();
  void GetBucketWebsite();
  void GetBucketReferer();
  void GetBucketLifecycle();
  void GetBucketStat();
  void GetBucketCors();

  void CleanAndDeleteBucket(const std::string &bucket);
  void DeleteBucketsByPrefix();
#ifdef USE_CPP20
  async_simple::coro::Lazy<void>
  test_coro_lazy(const AlibabaCloud::OSS::CreateBucketRequest &request);
#endif
private:
  void PrintError(const std::string &funcName,
                  const AlibabaCloud::OSS::OssError &error);
  AlibabaCloud::OSS::OssClient *client;
  std::string bucket_;
};
