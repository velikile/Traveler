
#include "hello.h"
//#include "h_encode_decode.h"
//#include "vector_functions.h"
//#include "priority_queue.h"

//have a 5 rooms level going up switches to top level
//if currently in up left bottom or right rooms
// don't let a move into another left room just clamp triangle position

enum rect_menu
{
	ZOOM,
	EDITRECT,
	SMALL,
	MED,
	LARGE,
	HOR,
	VER,
	RECT_MENU_COUNT,
};
struct dblclick
{
	bool firstclick; 
	bool idlephase;
	int idleduration;
	bool secondclick;
};
struct clickable
{
	Rect pos_size;
	bool visible;
	bool active;
	dblclick dblclickstate;
	rect_menu type;
	char * text;
	Color c;
};
struct edit_rect
{
	Rect *edited;
	bool tested;
};

struct acceleration_handle
{
	float acceleration;
	float topspeed;
	float minspeed;
	float speed;
	float speedX;
	float speedY;

};

int main(int argc,char ** argv)
{	
	clickable buttons [3];
	int heightCount = 0;
	int widthCount = 0;
	int buttonWidth = 30;
	int buttonHeight = 10; 

	edit_rect editedRects = {};

	for (int i = 0; i < arr_size(buttons); ++i)
	{
		if(widthCount*buttonWidth<width)
		{
			buttons[i].pos_size.x = widthCount++*buttonWidth;
			buttons[i].pos_size.w = buttonWidth;
			buttons[i].pos_size.h = buttonHeight;
		}
		else 
		{
			widthCount=0;
			heightCount++;
		}
		buttons[i].pos_size.y = heightCount*buttonHeight;

		buttons[i].active = false;
		buttons[i].dblclickstate = {0};

	}
	srand(time(0));
	fileData bmpFileData = readFile("testP.bmp");
	Image texture = produceRGBAPackedOutBMP(bmpFileData);
        if(bmpFileData.sizeinBytes > 0)
            free(bmpFileData.data);	
	//writeToFile("rawData",(char*)texture.data,texture.width*texture.height*sizeof(rgbapixel));
	Rect RectAddition ={0,0,20,20};

	BNode TreeHead ={0};

	for (int i = 0; i < tree_size; ++i)
	{
		int *val = mal(int,1);
		*val = random()%tree_size;
		AddValue(&TreeHead,val,intcompr,initNodeInt);
	}
	printf("tree_generated\n");


	// printf("start of btree_print\n");
	// inOrderPrint(&TreeHead,printInt);
	// printf("end of btree_print\n");

	LLRect squereCenter={0};

	LLRect * currentList = &squereCenter;

	int rectCounter=0;
	
	int amountOfRects =300;
	Rect *sceneRects = mal(Rect,amountOfRects);
	SDL_Color *rectColor = mal(Color,amountOfRects);
	// unified coordinates system

	Rect Trect;
	
	for (int i = 0; i < amountOfRects; i++)
	{
		int x = random()%2000;
		int y = random()%2000;
		int w = random()%200+10;
		int h = random()%200+10;

		sceneRects[i].x=x;
		sceneRects[i].y=y; 
		sceneRects[i].w=w;
		sceneRects[i].h=h;
		rectColor[i].r = w*19;
		rectColor[i].g = h*23;
		rectColor[i].b = x*y*27;

	}

	SDL_Window* w = NULL;
    //The surface contained by the window
	SDL_Surface* screenSurface = NULL;
   //Initialize SDL
	SDL_Renderer* r = NULL;

	float roll = 0;
	
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }

    SDL_CreateWindowAndRenderer(width, height, 0, &w, &r);
	SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
	screenSurface = SDL_GetWindowSurface(w);
 
	printf("format : %s\n",SDL_GetPixelFormatName(screenSurface->format->format));

	SDL_bool done = SDL_FALSE;

	float x = 320,y = 200,x0 = 300,y0 = 240,x1 = 360,y1 = 240;

	Rect * grabbedRect =0;
	Rect * toggleRect =mal(Rect,1);
	Rect * bigRect =  mal(Rect,1);
	Rect * medRect =  mal(Rect,1);
	Rect * smallRect = mal(Rect,1);

	smallRect->x = 20;
	smallRect->y = 20;
	smallRect->w =50;
	smallRect->h = 50;

	medRect->x = 20;
	medRect->y = 75;
	medRect->w = 70;
	medRect->h = 70;

	bigRect->x = 20;
	bigRect->y = 150;
	bigRect->w = 100;
	bigRect->h = 100;

	bool RectMenuVisible = true;
	SDL_Texture *tx = SDL_CreateTexture(r, SDL_PIXELFORMAT_BGRA8888,
										   SDL_TEXTUREACCESS_TARGET,
										   texture.width,
										   texture.height);

	SDL_Texture *tx0 = SDL_CreateTexture(r, SDL_PIXELFORMAT_BGRA8888,
										   SDL_TEXTUREACCESS_TARGET,
										   width,
										   height);

	SDL_Texture *tx1 = SDL_CreateTexture(r, SDL_PIXELFORMAT_BGRA8888,
										    SDL_TEXTUREACCESS_TARGET,
										    tree_texture_width,
										    tree_texture_height);



	V2 mousePos={0};
	B3 mouseBState ={0};
	V2 mouseDir={0};
	float midPointX = (x+x0+x1)/3;
	float midPointY = (y+y0+y1)/3;
	float dirX =0,dirY = 0;

	bool increaseRect = true;
	bool colissionRect  = false;
	bool holdingRect = false;
	Rect * heldRect = 0;
	int projX = 0,projY = 0;
	Rect rectProj;
	V2 Pdir;
	Rect zoom = {width/2-100,height/2-100,200,200};
	bool RenderProjectile = false;
	bool toggleVisible = false;
	bool toggleZoom = false;
	bool idle = true;
	//______________________________________________________________________
	// keys
	bool 	left = false,
			right=false,
			up=false,
			down=false,
			wkey=false,
			skey = false,
			xkey= false,
			ckey= false,
			ekey= false,
			zkey= false,
			vkey= false,
			akey= false,
			rkey= false,
			space=false,
			fkey = false,
			qkey = false,
			ctrl = false;



	// I would like it to be at one at most two locations 
	// what : the thing that manages the render screen for each rectangle 
	// guess that the thing has to be a list or stack 
	// just have a limit for each screen and when you start to push more it 

	int lastTimeHoldingPressed =0;
	int lastTimeIncreasePressed =0;
	int lastTimeSpeedChange=0;
	int lastTimeToggleVisible = 0;
	int lastTimeZoomKeyPressed = 0;
	int lastTimeRectColission=0;
	int lastTimeAddRect=0;
	int glideCameraTime=0;
	int switchTextureTime=0;

	float glideDt = 0;

	acceleration_handle ah = {};
	ah.acceleration = 0.001f;
	ah.topspeed = .2f;
	ah.minspeed = 0;
		
	// Rect recttorender = {0,0,texture.width,texture.height};  
	Rect recttorender = {0,0,texture.width,texture.height};  
	SDL_UpdateTexture(tx,
            		  &recttorender,
            		  texture.data,
            		  texture.width*4);

	SDL_SetTextureBlendMode(tx,SDL_BLENDMODE_BLEND);

	worldRect * boundingRects = mal(worldRect,(worldRectsWidth*worldRectsHeight));

	for (int i = 0; i < worldRectsWidth*worldRectsHeight; ++i)
	{
		int Ry = i/worldRectsHeight;
		int Rx = i%worldRectsWidth;
		boundingRects[i].pos = R(Rx*width,Ry*height,width,height);
	}
	int rectSpin=0;
	int rotateSpeed =2;
	bool moving = false;

	V2p shipState= {width/2,height/2};
	//shipState.p.x =width/2,shipState.p.y = height/2;
	int startFrameTime = 0;
	int  baseFrameRate = 16;

	V2 tempPos = {tree_texture_width/2,20};

	//printf("tree_texture width/log(width) = %d\n",((tree_texture_width*2)/(int)log(tree_texture_width)));
	//printf("log(width) = %d\n",(int)log(tree_texture_width));

	V2p leftRightPair = {V(-((width)/((int)log(tree_texture_width)-2)-5),
							 (height)/(TreeHead.height+1)),
						 V( ((width)/((int)log(tree_texture_width)-2)-5),
						 	 (height)/(TreeHead.height+1))};

	//render_btree(r,tx1,&TreeHead,tempPos,leftRightPair,simple_btree_renderer);
	render_btree(r,tx1,&TreeHead,tempPos,leftRightPair,simple_btree_renderer);


	//inOrderPrint(&h_Tree,printChar);

	// DQ(int) TestQueue ={0};
	// init_back_front(&TestQueue);
	// for (int i = 1; i < 11; ++i)
	// {
	// 	push_back(i,&TestQueue);
	// 	push_front(i*2,&TestQueue);
	// }
	// int count = 0;
	// while(!is_empty(&TestQueue))
	// {
	// 	printf("%d %d,",count++,pop_front(&TestQueue));
	// 	printf("%d %d,",count++,pop_back(&TestQueue));
	// }
	// printf("\n");
	//printf("%d,",pop_front(&TestQueue));
	//printf("%d,",pop_front(&TestQueue));
	//printf("%d,",pop_front(&TestQueue));
	

    while (!done) 
    {
    	startFrameTime = SDL_GetTicks();
        SDL_Event event;
        V2 glideDtXY = {glideDt*.3f*dirX,glideDt*.3f*dirY};
        V2 toAddRender = {width/2-shipState.a.x,height/2-shipState.a.y};
        toAddRender+=glideDtXY;
        //Rect dest = {midPointX +toAddRender.x-50,midPointY+toAddRender.y-50,100,100};
        Rect dest = {0,0,width,height};

       	SDL_SetRenderTarget(r,tx0);
       	SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
       	SDL_RenderClear(r);
		SDL_SetRenderDrawColor(r, 255, 0, 0, SDL_ALPHA_OPAQUE);	

       	if(toggleVisible)
       	{
       		SDL_RenderFillRect(r,toggleRect);
       	}
       	if(RectMenuVisible)
       	{
       		SDL_SetRenderDrawColor(r, 0, 200, 0, SDL_ALPHA_OPAQUE);
       		if(mousePos>(*smallRect) &&!grabbedRect)
       			SDL_RenderFillRect(r,smallRect);
       		else
       			SDL_RenderDrawRect(r,smallRect);
       		if(mousePos>(*medRect)&&!grabbedRect)
       			SDL_RenderFillRect(r,medRect);
       		else 
       			SDL_RenderDrawRect(r,medRect);
       		if(mousePos>(*bigRect)&&!grabbedRect)
       			SDL_RenderFillRect(r,bigRect);
       		else 
       			SDL_RenderDrawRect(r,bigRect);
       	}
		//_______________________________________________________________________________
       	//rects loop

        for (int i = 0; i < amountOfRects; ++i)
        {
        	if(i<worldRectsHeight*worldRectsWidth)
        	{	
        		float t = (float)i/(worldRectsWidth*worldRectsHeight);
        		int alpha = 255.f * t + (1.f-t)*128.f;
        		if(shipState.p>boundingRects[i].pos)
        		{
        		 	SDL_SetRenderDrawColor(r,255,255,255, 0);
        		 	currentList = &(boundingRects[i].head);
        		 }
        		 else 
        		 	SDL_SetRenderDrawColor(r,255,255,255, alpha);
        		 Rect temp = (boundingRects+i)->pos+toAddRender;
        		 SDL_RenderFillRect(r,&temp);
        		 SDL_SetRenderDrawColor(r,255,0,0, SDL_ALPHA_OPAQUE);
        		 SDL_RenderDrawRect(r,&temp);
        	}
        	if(sceneRects[i].x==0 &&sceneRects[i].y ==0)
        		continue;
        	if(getDistanceFromPoint(sceneRects[i],shipState.p)>=500)
        		continue;
        	

        	Rect serpCenter=sceneRects[i];

        	serpCenter+=toAddRender;

        	if(buttons[EDITRECT].active)
        	{
        		if(mouseBState.l && mousePos>serpCenter && !editedRects.edited)
    			{	
    				editedRects.edited = sceneRects + i;
    				editedRects.edited->x = mousePos.x - toAddRender.x-sceneRects[i].w/2;
    				editedRects.edited->y = mousePos.y - toAddRender.y-sceneRects[i].h/2;
    			}
    			else if (mouseBState.l && editedRects.edited)
    			{
    				editedRects.edited->x = mousePos.x - toAddRender.x-editedRects.edited->w/2;
    				editedRects.edited->y = mousePos.y - toAddRender.y-editedRects.edited->h/2;
    			}
    			else if (!mouseBState.l && editedRects.edited)
    			{
    					editedRects.edited = 0;
    			}
        	}

        	
        	
			SDL_SetRenderDrawColor(r,rectColor[i].r,rectColor[i].g,rectColor[i].b, SDL_ALPHA_OPAQUE);

			if(heldRect != sceneRects+i)
				SDL_RenderDrawRect(r,&serpCenter);
			else 
				SDL_RenderFillRect(r,&serpCenter);
			if(xkey && timeToUpdate(lastTimeHoldingPressed,100,true))
			{
				// printf("holdingRect %d\n",!holdingRect);
				if(holdingRect)
				{					
					heldRect = 0;
				}
				holdingRect = !holdingRect;
			}
			if(RenderProjectile&&inRect(sceneRects[i],rectProj.x-toAddRender.x,rectProj.y-toAddRender.y))
			{
				//printf("%d,%d,%d,%d,%d,%d\n",x,countPerScreen,sceneRects[x].x,sceneRects[x].y,sceneRects[x].w,sceneRects[x].h);
				rectColor[i].r-=3;
				RenderProjectile = false;
			}
			if(inRect(sceneRects[i],Trect))
			{	
				if(holdingRect && !timeToUpdate(lastTimeHoldingPressed,60,false))
				{
					heldRect = sceneRects + i;
				}
				else if (!holdingRect && !timeToUpdate(lastTimeHoldingPressed,60,false))
				{
					heldRect = 0;
				}
				
				V2 DirVec = V(dirX,dirY);
				V2 A = V(x0,y0)+toAddRender;//+DirVec;
				V2 B = V(x,y)  +toAddRender;//+DirVec;
				V2 C = V(x1,y1)+toAddRender;//+DirVec;
				Rect rectToTest ={sceneRects[i].x+toAddRender.x,
							      sceneRects[i].y+toAddRender.y,
								  sceneRects[i].w,
								  sceneRects[i].h};
				V2p S = getIntersectionPoints(rectToTest,A,B,C,DirVec);
				
				if(S.active)
				{
					V2 toAdd = perp(S.a-S.b);
					normalize(&toAdd);
					float da = getDistanceFromLine(S.a,S.b,A);
					float db = getDistanceFromLine(S.a,S.b,B);
					float dc = getDistanceFromLine(S.a,S.b,C);
					bool inA = inRect(rectToTest,A),
					     inB = inRect(rectToTest,B),
						 inC = inRect(rectToTest,C);
					
					if(inA&&inB){toAdd*=min(-da,-db);}
					else if(inA&&inC){toAdd*=max(-da,-dc);}
					else if(inA){toAdd*=-da;}
					else if(inB&&inC){toAdd*=min(-db,-dc);}
					else if(inB){toAdd*=-db;}
					else if(inC){toAdd*=-dc;}

					if(!inA && !inB && !inC)
					{
						V2q RectVecs = getRectVecs(rectToTest);
						V2 nextPoint ={rectToTest.x,rectToTest.y};

						for (int j = 0; j < 4; ++j)
						{
							if(inRect(Trect+toAddRender,nextPoint))
							{
								float d = getDistanceFromLine(S.a,S.b,nextPoint);
								toAdd*=d;
								break;
							}
							nextPoint = nextPoint+RectVecs.data[j]; 
						}
					}
					
					SDL_SetRenderDrawColor(r, 255, 0, 255, SDL_ALPHA_OPAQUE);
					SDL_RenderDrawLine(r, midPointX+toAddRender.x, midPointY+toAddRender.y, midPointX + toAdd.x+toAddRender.x,midPointY + toAdd.y+toAddRender.y);

					if(colissionRect)
					{					
						x+=toAdd.x;
						y+=toAdd.y;
						x0+=toAdd.x;
						y0+=toAdd.y;
						x1+=toAdd.x;
						y1+=toAdd.y;
					}
				}
				 SDL_SetRenderDrawColor(r, 255, 255, 0, SDL_ALPHA_OPAQUE);
				 Rect renderCol = {Trect.x+toAddRender.x,Trect.y+toAddRender.y,Trect.w,Trect.h};
				 // SDL_RenderDrawRect(r,&Trect);
				 SDL_RenderDrawRect(r,&renderCol);
        	}
    	}

    	//_______________________________________________________________________________
        if(holdingRect && !heldRect)
        	holdingRect = false;

        SDL_SetRenderDrawColor(r, 255, 255, 255, SDL_ALPHA_OPAQUE);
        //draw a triangle 

        int minX =min(min(x,x0),x1);
        int minY =min(min(y,y0),y1);
        int maxX =max(max(x,x0),x1);
        int maxY =max(max(y,y0),y1);
       	Trect.x = minX,
        Trect.y = minY,
        Trect.w = maxX-minX,
        Trect.h = maxY-minY;
    
        SDL_RenderDrawLine(r, x+toAddRender.x, y+toAddRender.y, x0+toAddRender.x,y0+toAddRender.y);
        SDL_RenderDrawLine(r, x0+toAddRender.x, y0+toAddRender.y, x1+toAddRender.x,y1+toAddRender.y);
        SDL_RenderDrawLine(r, x1+toAddRender.x, y1+toAddRender.y, x+toAddRender.x,y+toAddRender.y);

        if(moving && timeToUpdate(glideCameraTime,baseFrameRate*2,true) && glideDt<100)
    		glideDt++;
        else if(!moving && glideDt>0)
        	glideDt--;
        

        while (SDL_PollEvent(&event)) 
        {
        	switch( event.type )
        	{
        	 	case SDL_MOUSEMOTION:
        	 		mousePos.x = event.motion.x;
        	 		mousePos.y = event.motion.y;
        	 		mouseDir.x = event.motion.xrel;
        	 		mouseDir.y = event.motion.yrel;
        	 		break;
        	 	case SDL_MOUSEBUTTONUP:
        	 	case SDL_MOUSEBUTTONDOWN:
    	 			mouseDir.x = 0;
    	 			mouseDir.y = 0;
    	 			mouseBState.r = event.button.button == SDL_BUTTON_RIGHT && event.type ==SDL_MOUSEBUTTONDOWN;
    	 			mouseBState.m = event.button.button == SDL_BUTTON_MIDDLE && event.type ==SDL_MOUSEBUTTONDOWN;
    	 			mouseBState.l = event.button.button == SDL_BUTTON_LEFT && event.type ==SDL_MOUSEBUTTONDOWN;
        	 		break;
        	 	case SDL_KEYUP:
		     	case SDL_KEYDOWN:
		     		 switch( event.key.keysym.sym )
		     		 {
		     		 	// ctrl = event.key.keysym.mod & KMOD_LCTRL;
		     		 	case SDLK_LCTRL:
		     		 		ctrl = (event.type==SDL_KEYDOWN);
		     		 		break;
	                    case SDLK_LEFT:
	                        left = (event.type==SDL_KEYDOWN);
	                        break;
	                    case SDLK_RIGHT:
	                        right = (event.type==SDL_KEYDOWN);
	                        break;
	                    case SDLK_UP:
	                        up = (event.type==SDL_KEYDOWN);
	                        break;
	                    case SDLK_DOWN:
	                        down = (event.type==SDL_KEYDOWN);
	                        break;
                        case SDLK_w:
                        	wkey = (event.type==SDL_KEYDOWN);
                        	break;
                    	case SDLK_s:
	                    	skey = (event.type==SDL_KEYDOWN);
	                    	break;
	                    case SDLK_x:
	                        xkey = (event.type==SDL_KEYDOWN);
                        break;
                        case SDLK_e:
	                        ekey = (event.type==SDL_KEYDOWN);
                        break;
                        case SDLK_c:
	                        ckey = (event.type==SDL_KEYDOWN);
                        break;
                       	case SDLK_z:
	                        zkey = (event.type==SDL_KEYDOWN);
                        break;
                        case SDLK_v:
	                        vkey = (event.type==SDL_KEYDOWN);
                        break;
                        case SDLK_a:
	                        akey = (event.type==SDL_KEYDOWN);
                        break;
                        case SDLK_r:
	                        rkey = (event.type==SDL_KEYDOWN);
	                    break;
	                    case SDLK_SPACE:
                       		space= (event.type==SDL_KEYDOWN);
                       		break;
                       	case SDLK_f:
                       		fkey = (event.type==SDL_KEYDOWN);
                       		break;
                        case SDLK_q:
                       		qkey = (event.type==SDL_KEYDOWN);

                       		if (qkey && event.key.keysym.mod & KMOD_LCTRL){printf("ctrl q\n");}
                       	break;
	                    default:
	                    	idle = true;
	                        break;
                	}
		      	break;		      
		        case SDL_QUIT:
		          done = SDL_TRUE;
		          break;
		    }
        }
		
		midPointX = (x+x0+x1)/3;
		midPointY = (y+y0+y1)/3;

		if(left||up||right)
		{
                    lastTimeSpeedChange = SDL_GetTicks();
                    ah.speedX += (ah.speedX < ah.topspeed) *(ah.acceleration * left - ah.acceleration * right);
                    ah.speedY += (ah.speedY < ah.topspeed) * ah.acceleration * up;
		}
		else 
                {
                    if(ah.speedX > ah.minspeed)
                    {	
			ah.speedX -= ah.acceleration;
			if(ah.speedX < ah.minspeed)
                            ah.speedX = ah.minspeed;
                    }
                    if(ah.speedY > ah.minspeed)
                    {	
                        ah.speedY -= ah.acceleration;
                        if(ah.speedY < ah.minspeed)
                            ah.speedY = ah.minspeed;
                    }
                }

        	//printf("%.3f,%.3f,%.3f,%.3f\n",dirX,dirY,ah.speed,ah.acceleration);
//        	if(holdingRect && heldRect)
 //       		(*heldRect)+=V(dirX,dirY);

        updateState(ekey,lastTimeRectColission,150,colissionRect);
      	updateState(qkey,lastTimeToggleVisible,100,RectMenuVisible);
        updateState(vkey,lastTimeIncreasePressed,100,increaseRect);
        updateState(fkey,lastTimeZoomKeyPressed,100,toggleZoom);
        
        //if this button was pressed in the last 30 ms then w

        if(RectMenuVisible)
	        for (int k = 0; k < arr_size(buttons); ++k)
	       	{
	       		if(mouseBState.l &&
	       			mousePos>buttons[k].pos_size)
	       		{
	       			if(buttons[k].dblclickstate.firstclick&&
	       				buttons[k].dblclickstate.idlephase)
	       			{
	       				buttons[k].dblclickstate = {0};
	       				buttons[k].active = !buttons[k].active;
	       			}
	       			else
	       				buttons[k].dblclickstate.firstclick = true;
	       		}
	       		else if(!mouseBState.l && 
	       				mousePos>buttons[k].pos_size)
	       		{	
	       			if(buttons[k].dblclickstate.firstclick)
	       			{
	       				if(buttons[k].dblclickstate.idlephase == true)
	       					buttons[k].dblclickstate.idleduration++;
	       				if(buttons[k].dblclickstate.idleduration > 5)
	       					buttons[k].dblclickstate = {0};	
	       				else 
	       					buttons[k].dblclickstate.idlephase = true;
	       			}
	       		}
		       	if(buttons[k].active)
		       	{
		       		Color cb = buttons[k].c;
		       		SDL_SetRenderDrawColor(r, 255, 255, 0, SDL_ALPHA_OPAQUE);
		       		SDL_RenderFillRect(r,&buttons[k].pos_size);
		       	}
		       	else 
		       		SDL_RenderDrawRect(r,&buttons[k].pos_size);
	       	}
	       	if(buttons[ZOOM].active)
	       	{
				SDL_RenderDrawRect(r,&zoom);
				if(mouseBState.l && mousePos>zoom && !ctrl)
				{
					zoom.x = mousePos.x-zoom.w/2;
					zoom.y = mousePos.y-zoom.h/2;
				}
				else if(mouseBState.l && ctrl)
				{
					zoom.w =  mousePos.x - zoom.x;
					zoom.h =  mousePos.y - zoom.y;
				}
	       	}

         if(akey && timeToUpdate(lastTimeAddRect,10,true))
        {
			printf("RectAddition = %d\n",rectCounter++);
			LLRect *addition = (LLRect*)malloc(sizeof(LLRect));
			addition->e = RectAddition;
			addition->next =0;
			addition->e.x = x;
			addition->e.y = y;
			AddToList(addition,currentList);
        }
        if(rkey && timeToUpdate(lastTimeAddRect,100,true))
        {
			if(currentList)
			{
				if(RemoveLastListElement(currentList))
				{
					printf("RectAddition = %d\n",(--rectCounter));
				}	
			}
        }
        if(holdingRect && heldRect)
        {
        	heldRect->w+=((increaseRect)|(!increaseRect*-1))*zkey;
        	heldRect->h+=((increaseRect)|(!increaseRect*-1))*ckey;
        	if(heldRect->w<0)
            	heldRect->w=0;
            if(heldRect->h<0)
            	heldRect->h=0;
        }
       	if (space && !RenderProjectile)
       	{
       		RenderProjectile = true;
       		Pdir.x=(x1-midPointX)/3;Pdir.y = (y1-midPointY)/3;
       		projX = 0;
       		projY = 0;
       	}
       	if(RenderProjectile)
       	{
			rectProj.x = toAddRender.x+midPointX+projX;
			rectProj.y = toAddRender.y+midPointY+projY;
			rectProj.w = 3;
			rectProj.h = 3;
			SDL_RenderDrawRect(r,
        	                &rectProj);
			if(rectProj.y>height || rectProj.x>width)
			{
				RenderProjectile = false;
			}
			else if(rectProj.y<0 || rectProj.x<0)
			{
				RenderProjectile = false;
			}
			if(RenderProjectile)
			{
				projX+=Pdir.x;
				projY+=Pdir.y;
			}
       	}
       	if (wkey || skey)
       	{
            rotate_ship(x,y,x0,y0,x1,y1,midPointX,midPointY,wkey * -0.05 + skey* 0.05);
            shipState.d.x =dirX;
            shipState.d.y= dirY;
       	    dirX = (x1-midPointX)*ah.speedX;
       	    dirY = (y1-midPointY)*ah.speedY;
            render_fire(r,shipState,dirX,dirY,toAddRender,0,10,30,100);
     	}
       	
       	if(currentList)
       	{
       		SDL_Texture * Trand;
       		if(timeToUpdate(switchTextureTime,5000,true))
       		{
       			int selection = rand()%3;
       			if(selection==0)
       				Trand = tx;
       			else if(selection ==1)
       			{
					Trand = tx1;
       			}
       			else
       			{
       				Trand = tx0;
       			} 	
       		}
       		drawListRectsAndChangePositionForDraggedMousePosition(r,
                                                                      currentList,
                                                                      mouseBState,
                                                                      mousePos,
                                                                      &grabbedRect,
                                                                      Trand,
                                                                      toAddRender);
       	}
       	if(ah.speedX > ah.minspeed || ah.speedY > ah.minspeed)
        {
            dirX =  (x1 - midPointX) * ah.speedY;
            dirY =  (y1 - midPointY) * ah.speedY;
            translateTriangle(x,y,x0,y0,x1,y1,dirX,dirY);
            render_fire(r,shipState,dirX,dirY,toAddRender,0,15,0,300);
        }

       	if(mouseBState.l)
       	{
       		if(grabbedRect)
       		{
       			grabbedRect->x = mousePos.x-toAddRender.x-grabbedRect->w/2;
       			grabbedRect->y = mousePos.y-toAddRender.y-grabbedRect->h/2;
       		}
       		else if (RectMenuVisible && currentList && 
       			     timeToUpdate(lastTimeAddRect,100,true))
       		{
       			Rect * e = (smallRect && (mousePos>(*smallRect)))?smallRect:
       						(medRect &&(mousePos>(*medRect)))? medRect :
       						 (bigRect &&(mousePos>(*bigRect)))? bigRect: 0;
       			if(e != 0)
       			{
	       			LLRect *addition = mal(LLRect,1);
	       			addition->e = *e;
					addition->next =0;
					addition->e.x = mousePos.x-toAddRender.x-e->w/2;
					addition->e.y = mousePos.y-toAddRender.y-e->h/2;				
	       			AddToList(addition,currentList);
       			}
       		}
       	}
       	if(grabbedRect && mouseBState.r)
       	{
       		int distX = mousePos.x - grabbedRect->x-toAddRender.x;
			int distY = mousePos.y - grabbedRect->y-toAddRender.y;
       		grabbedRect->w = max(0,distX) ;
       		grabbedRect->h = max(0,distY);
       	}
       	if(grabbedRect && mouseBState.m)
       	{
       		RemoveFromList(grabbedRect,currentList);
       	} 
     	
     	shipState.a.x = midPointX;
     	shipState.a.y = midPointY;

		SDL_SetRenderTarget(r,NULL);
		if(!toggleZoom)
			SDL_RenderCopy(r, tx0, 0,0);
		else
			SDL_RenderCopy(r, tx0, &zoom,0);
		
       	SDL_RenderPresent(r);


       	int FrameTime = SDL_GetTicks()-startFrameTime;
       	if(baseFrameRate>FrameTime)
       		SDL_Delay(baseFrameRate-FrameTime);	
       	else 
       		printf("%d\n",FrameTime);
        

        //printf("%2f %2f\n",shipState.a.x,shipState.a.y);
    }
    // Pause execution for 3000 milliseconds, for example
    // Close and destroy the window
    SDL_DestroyWindow(w);
    // Clean up
    free(sceneRects);
    free(rectColor);
    free(boundingRects);
    SDL_Quit();
	return 0;
}



