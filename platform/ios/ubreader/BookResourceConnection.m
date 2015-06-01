#import "BookResourceConnection.h"

#import "BookResourceDataResponse.h"
#import "BookResourceServer.h"
#include "src/dal/base.hpp"

static __weak BookResourceServer *m_packageResourceServer = nil;

@implementation BookResourceConnection



- (NSObject <HTTPResponse> *)httpResponseForMethod:(NSString *)method URI:(NSString *)path {
	if (m_packageResourceServer == nil ||
		method == nil ||
		![method isEqualToString:@"GET"] ||
		path == nil ||
		path.length == 0)
	{
		return nil;
	}
		
	NSObject <HTTPResponse> *response = nil;
    
    bool isShelfView = [m_packageResourceServer isActiveShelfView];
    
    BookResourceDataResponse *dataResponse = [[BookResourceDataResponse alloc]
                                                   initWithUri:path andActiveShelfViewType:isShelfView];
        
    response = dataResponse;
    return response;
}


+ (void)setPackageResourceServer:(BookResourceServer *)packageResourceServer {
	m_packageResourceServer = packageResourceServer;
}


@end
