
#import "NavigationElementController.h"
#import "EPubViewController.h"
#import "NavigationElementItem.h"
#import "RDContainer.h"
#import "RDNavigationElement.h"
#import "RDPackage.h"
#import "RDSpineItem.h"
#import "RDContainer.h"



@interface NavigationElementController () {
	@private NSMutableArray *m_items;
   	@private NSMutableArray *m_sdkErrorMessages;
}

- (void)addItem:(NavigationElementItem *)item;

@end


@implementation NavigationElementController


- (void)addItem:(NavigationElementItem *)item {
	if (item.level >= 16) {
		NSLog(@"There are too many levels!");
		return;
	}

	[m_items addObject:item];

	for (RDNavigationElement *e in item.element.children) {
		[self addItem:[[NavigationElementItem alloc]
			initWithNavigationElement:e level:item.level + 1]];
	}
}


- (instancetype)initWithPath:(NSString *)path {
    if (self = [super initWithTitle:nil navBarHidden:NO]) {
        
        m_sdkErrorMessages = [[NSMutableArray alloc] initWithCapacity:0];
        
        m_container = [[RDContainer alloc] initWithDelegate:self path:path];
        
        m_package = m_container.firstPackage;
        
        [self popErrorMessage];
        
        if (m_package == nil) {
            return nil;
        }
        
        m_element = m_package.tableOfContents;
        m_items = [[NSMutableArray alloc] init];
        
        for (RDNavigationElement *e in m_package.tableOfContents.children) {
            [self addItem:[[NavigationElementItem alloc] initWithNavigationElement:e level:0]];
        }
        
        NSArray *components = path.pathComponents;
        self.title = (components == nil || components.count == 0) ? @"" : components.lastObject;
    }
    
    return self;
}
- (void) popErrorMessage
{
    NSInteger count = [m_sdkErrorMessages count];
    if (count > 0)
    {
        __block NSString *message  = [m_sdkErrorMessages firstObject];
        [m_sdkErrorMessages removeObjectAtIndex:0];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
            UIAlertView * alert =[[UIAlertView alloc]
                                  initWithTitle:@"EPUB warning"
                                  message:message
                                  delegate: self
                                  cancelButtonTitle:@"Ignore all"
                                  otherButtonTitles: nil];
            [alert addButtonWithTitle:@"Ignore"];
            [alert show];
        });
    }
}

- (id)
	initWithNavigationElement:(RDNavigationElement *)element
	container:(RDContainer *)container
	package:(RDPackage *)package
	title:(NSString *)title
{
	if (element == nil || container == nil || package == nil) {
		return nil;
	}

	if (self = [super initWithTitle:title navBarHidden:NO]) {
		m_container = container;
		m_element = element;
		m_items = [[NSMutableArray alloc] init];
		m_package = package;

		for (RDNavigationElement *e in element.children) {
			[self addItem:[[NavigationElementItem alloc] initWithNavigationElement:e level:0]];
		}
	}

	return self;
}


- (void)loadView {
	self.view = [[UIView alloc] init];

	UITableView *table = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStylePlain];
	m_table = table;
	table.dataSource = self;
	table.delegate = self;
	[self.view addSubview:table];
}


- (UITableViewCell *)
	tableView:(UITableView *)tableView
	cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
		reuseIdentifier:nil];

	NavigationElementItem *item = [m_items objectAtIndex:indexPath.row];

	cell.indentationLevel = item.level;

	cell.textLabel.text = [item.element.title stringByTrimmingCharactersInSet:[NSCharacterSet
		whitespaceAndNewlineCharacterSet]];

	if (item.element.content == nil || item.element.content.length == 0) {
		cell.selectionStyle = UITableViewCellSelectionStyleNone;
		cell.textLabel.textColor = [UIColor colorWithWhite:0 alpha:0.5];
	}
	else {
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	}

	return cell;
}


- (void)
	tableView:(UITableView *)tableView
	didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	NavigationElementItem *item = [m_items objectAtIndex:indexPath.row];

	if (item.element.content != nil && item.element.content.length > 0) {
		EPubViewController *c = [[EPubViewController alloc]
			initWithContainer:m_container
			package:m_package
			navElement:item.element];

		if (c != nil) {
            CATransition *transition = [CATransition animation];
            transition.duration = 1.0;
            transition.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
            transition.type = kCATransitionPush;
            transition.subtype = kCATransitionFromBottom;
            transition.delegate = self;
            [self.navigationController.view.layer addAnimation:transition forKey:nil];
            
			[self.navigationController pushViewController:c animated:YES];
		}
	}
}

- (BOOL)container:(RDContainer *)container handleSdkError:(NSString *)message isSevereEpubError:(BOOL)isSevereEpubError{

    return isSevereEpubError;
}


- (NSInteger)
	tableView:(UITableView *)tableView
	numberOfRowsInSection:(NSInteger)section
{
	return m_items.count;
}


- (void)viewDidLayoutSubviews {
	m_table.frame = self.view.bounds;
}


- (void)viewWillAppear:(BOOL)animated {
	[super viewWillAppear:animated];

	if (m_table.indexPathForSelectedRow != nil) {
		[m_table deselectRowAtIndexPath:m_table.indexPathForSelectedRow animated:YES];
	}
}


@end
