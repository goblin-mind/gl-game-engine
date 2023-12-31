#include "Object3D.h"
int Object3D::idMod=0;
void Object3D::applyGravity()
{
	bool firmGround=onSolid();
	if(firmGround)
	{
		dir.dy=0;
		gravity=0;
		return;
	}
	if(dir.dy>=0 && !firmGround)
		gravity=default_gravity;
	
		
			
	dir.dy-=gravity;
	gravity*=1.01;
	//if(dir.getMag()!=0)
	//	dir=dir.normalize();

}
bool Object3D::onSolid()
{

	bool result=false;

	Container3D* bounds=super;
	if(super->super!=0)
		bounds=super->super;

	if((center()-(dim*0.5)).dy <= bounds->pos.dy ){
		pos.dy=bounds->pos.dy;
		result=true;
	}
	return result;
}
bool Stackable::onSolid()
{
	if(super==NULL)
		return 0;
	bool result=false;

	for(std::list<Object3D*>::iterator it=super->objects.begin();it!=super->objects.end();++it)
	{
		if( !(*this==**it) && (*it)->intersects(*this))
		{
			(*it)->forceOut(*this);	
			//forceOut(**it);
			
			if(center().dy-dim.dy*0.5>=(*it)->center().dy+(*it)->dim.dy*0.5)
				result=true;
		}
	}

	if(Object3D::onSolid())
		result=true;

	return result;
}

void Player::handleCollision(Object3D& b)
{
 	Object3D::handleCollision(b);
	b.actOn(*this);
}
void Object3D::handleCollision(Object3D& b)
{
	//if(!converging(b))
	//	return;
	//if(intersects(b) && converging(b))
	//	b.stepBack();
		
	Vector3D dif=b.center()-center();

		if(abs(dif.dx)>abs(dif.dy) && abs(dif.dx)>abs(dif.dz)) //x axis is the normal
			b.dir.dx=b.dir.dx*-1;
		else if(abs(dif.dy)>abs(dif.dx) && abs(dif.dy)>abs(dif.dz)) //x axis is the normal
			b.dir.dy=b.dir.dy*-1;
		else if(abs(dif.dz)>abs(dif.dy) && abs(dif.dz)>abs(dif.dx)) //x axis is the normal
			b.dir.dz=b.dir.dz*-1;
		
			forceOut(b);
		
		
	//}
		
}
void Sphere3D::handleCollision(Object3D& b)
{


		Vector3D v=Vector3D(b.center()-center());
		v=v.normalize()*(bindingRadius+b.bindingRadius-v.getMag());
		pos=pos-v;

		Vector3D xA = Vector3D(b.pos-pos);
		xA=xA.normalize();

		Vector3D U1x= xA*(xA.dot(dir));
		Vector3D U1y= dir-(U1x);
		Vector3D U2x =xA.invert()*(xA.invert().dot(b.dir));
		Vector3D U2y =b.dir-(U2x);

		Vector3D V1x=((U1x)+((U2x))-(U1x-(U2x)))*(0.5);
		Vector3D V2x=((U1x)+((U2x))-(U2x-(U1x)))*(0.5);
		
		
		dir=Vector3D(V1x+U1y).normalize();
		b.dir=Vector3D(V2x+U2y).normalize();



}
void Object3D::forceOut(Object3D& b)
{
	Vector3D dif=b.center()-center();
	//Vector3D normalDir=b.dir;

	if(abs(dif.dx)>abs(dif.dy) && abs(dif.dx)>abs(dif.dz))
		b.pos.dx+=signOf(dif.dx)*((b.bindingRadius+bindingRadius)-abs(dif.dx));
	else if(abs(dif.dz)>=abs(dif.dy) && abs(dif.dz)>=abs(dif.dx))
		b.pos.dz+=signOf(dif.dz)*((b.bindingRadius+bindingRadius)-abs(dif.dz));
	else if(abs(dif.dy)>=abs(dif.dx) && abs(dif.dy)>=abs(dif.dz))
		b.pos.dy+=signOf(dif.dy)*((b.bindingRadius+bindingRadius)-abs(dif.dy));
		
	b.actOn(*this);
}

bool Object3D::forceInBounds()
{
	bool result=false;
	Container3D *bounds=super;
	if(super->super!=0)
		bounds=super->super;
 	
	if( pos.dx>bounds->pos.dx+bounds->dim.dx){//if reach x bounds, reverse{
		forceNegative(dir.dx);
		result=true;
	}
	else if( pos.dx<bounds->pos.dx){
		forcePositive(dir.dx);
		result=true;
	}
	if(pos.dy<bounds->pos.dy){
		forcePositive(dir.dy);
		result=true;
	}
	else if ( pos.dy>bounds->pos.dy+bounds->dim.dy){
		forceNegative(dir.dy);
		result=true;
	}
	if(pos.dz>bounds->pos.dz+bounds->dim.dz){
		forceNegative(dir.dz);
		result=true;
	}
	else if ( pos.dz<bounds->pos.dz){
		forcePositive(dir.dz);
		result=true;
	}
	return result;

}
void Object3D::update()
{
	applyGravity();
	stepForward();
};
void Sphere3D::update()
{
	
	if(health>=0){
	
	detectCollisions();
	forceInBounds();
	stepForward();
	takeDamage();
	
	}
	
};
void NPC::actOn(Object3D& p)
{
	p.takeDamage();
}
void Object3D::stepForward(){
	pos=pos+(dir*speed);
}

void Object3D::stepBack(){pos=pos-(dir*speed);}
void Object3D::forceNegative(double &f){if(f>0)	f*=-1;}
void Object3D::forcePositive(double &f){if(f<0)	f*=-1;}

void Object3D::draw()
{

		double dl=dim.dx/2;
		double dh=dim.dy/2;
		double dd=dim.dz/2;
		
		glBindTexture(GL_TEXTURE_2D,TX.getTX());
		glPushMatrix();
		glTranslatef(center().dx, center().dy, center().dz);
		glNormal3f(0,0,1);
		glBegin(GL_QUADS); 
		glTexCoord2f(1.0f, 1.0f);glVertex3f( dl, dh, dd); //up right corner
		glTexCoord2f(0.0f, 1.0f);glVertex3f( -dl, dh, dd); //up left corner
		glTexCoord2f(0.0f, 0.0f);glVertex3f(-dl,-dh, dd); //down left
		glTexCoord2f(1.0f, 0.0f);glVertex3f( dl,-dh, dd);//down right
		glEnd();
		glNormal3f(0,0,-1);
		glBegin(GL_QUADS); 
		glTexCoord2f(1.0f, 1.0f);glVertex3f( dl, dh, -dd); //up right corner
		glTexCoord2f(0.0f, 1.0f);glVertex3f( -dl, dh, -dd); //up left corner
		glTexCoord2f(0.0f, 0.0f);glVertex3f(-dl,-dh, -dd); //down left
		glTexCoord2f(1.0f, 0.0f);glVertex3f( dl,-dh, -dd);//down right
		glEnd();
		glNormal3f(0,1,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 0.0f);glVertex3f( dl, dh, dd); //up right corner
		glTexCoord2f(1.0f, 0.0f);glVertex3f( -dl, dh, dd); //up left corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f(-dl,dh, -dd); //down left
		glTexCoord2f(0.0f, 1.0f);glVertex3f( dl,dh, -dd);//down right
		glEnd();
		glNormal3f(0,-1,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 0.0f);glVertex3f( dl, -dh, dd); //up right corner
		glTexCoord2f(1.0f, 0.0f);glVertex3f( -dl, -dh, dd); //up left corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f(-dl,-dh, -dd); //down left
		glTexCoord2f(0.0f, 1.0f);glVertex3f( dl,-dh, -dd);//down right
		glEnd();
		glNormal3f(1,0,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 0.0f);glVertex3f( dl, -dh, -dd); //up right corner
		glTexCoord2f(1.0f, 0.0f);glVertex3f( dl, -dh, dd); //up left corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f(dl,dh, dd); //down left
		glTexCoord2f(0.0f, 1.0f);glVertex3f( dl,dh, -dd);//down right
		glEnd();
		glNormal3f(-1,0,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 0.0f);glVertex3f( -dl, -dh, -dd); //up right corner
		glTexCoord2f(1.0f, 0.0f);glVertex3f( -dl, -dh, dd); //up left corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f(-dl,dh, dd); //down left
		glTexCoord2f(0.0f, 1.0f);glVertex3f( -dl,dh, -dd);//down right
		glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D,0);
		
}
void Container3D::draw()
{
	for(list<Object3D*>::iterator it=objects.begin();it!=objects.end();++it)
	{
		(*it)->draw();
	}
	if(draw_borders){
		double dl=dim.dx/2;
		double dh=dim.dy/2;
		double dd=dim.dz/2;

		glBindTexture(GL_TEXTURE_2D,TX.textures[0]);
		glPushMatrix();
		glTranslatef(pos.dx+dl, pos.dy+dh, pos.dz+dd);
		glNormal3f(0,0,-1);
		glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 0.0f);glVertex3f( dl, dh, dd); //up right corner
		glTexCoord2f(1.0f, 0.0f);glVertex3f( -dl, dh, dd); //up left corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f(-dl,-dh, dd); //down left
		glTexCoord2f(0.0f, 1.0f);glVertex3f( dl,-dh, dd);//down right
		glEnd();
		glNormal3f(0,0,1);
		glBegin(GL_QUADS); 
		glTexCoord2f(1.0f, 0.0f);glVertex3f( dl, dh, -dd); //up right corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f( -dl, dh, -dd); //up left corner
		glTexCoord2f(0.0f, 1.0f);glVertex3f(-dl,-dh, -dd); //down left
		glTexCoord2f(0.0f, 0.0f);glVertex3f( dl,-dh, -dd);//down right
		glEnd();
		glNormal3f(0,-1,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 1.0f);glVertex3f( dl, dh, dd); //up right corner
		glTexCoord2f(0.0f, 0.0f);glVertex3f( -dl, dh, dd); //up left corner
		glTexCoord2f(1.0f, 0.0f);glVertex3f(-dl,dh, -dd); //down left
		glTexCoord2f(1.0f, 1.0f);glVertex3f( dl,dh, -dd);//down right
		glEnd();
		glBindTexture(GL_TEXTURE_2D,TX.textures[1]);
		glNormal3f(0,1,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(1.0f, 1.0f);glVertex3f( dl, -dh, dd); //up right corner
		glTexCoord2f(0.0f, 1.0f);glVertex3f( -dl, -dh, dd); //up left corner
		glTexCoord2f(0.0f, 0.0f);glVertex3f(-dl,-dh, -dd); //down left
		glTexCoord2f(1.0f, 0.0f);glVertex3f( dl,-dh, -dd);//down right
		glEnd();
		glBindTexture(GL_TEXTURE_2D,TX.textures[0]);
		glNormal3f(-1,0,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(1.0f, 0.0f);glVertex3f( dl, -dh, -dd); //up right corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f( dl, -dh, dd); //up left corner
		glTexCoord2f(0.0f, 1.0f);glVertex3f(dl,dh, dd); //down left
		glTexCoord2f(0.0f, 0.0f);glVertex3f( dl,dh, -dd);//down right
		glEnd();
		glNormal3f(1,0,0);
		glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 0.0f);glVertex3f( -dl, -dh, -dd); //up right corner
		glTexCoord2f(1.0f, 0.0f);glVertex3f( -dl, -dh, dd); //up left corner
		glTexCoord2f(1.0f, 1.0f);glVertex3f(-dl,dh, dd); //down left
		glTexCoord2f(0.0f, 1.0f);glVertex3f( -dl,dh, -dd);//down right
		glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D,0);
	}
}
bool Object3D::converging(Object3D& b)
{
	double distanceBefore = sqrt(pow((center().dx-b.center().dx),2)+pow((center().dy-b.center().dy),2)+pow((center().dz-b.center().dz),2));
	double distanceAfter = sqrt(pow(((center().dx+dir.dx*speed)-(b.center().dx+b.dir.dx*b.speed)),2)+pow(((center().dy+dir.dy*speed)-(b.center().dy+b.dir.dy*b.speed)),2)+pow(((center().dz+dir.dz*speed)-(b.center().dz+b.dir.dz*b.speed)),2));

	if(distanceAfter-distanceBefore<=0)
		return true;
	return false;
}
bool Object3D::detectCollisions()
{
	bool result=false;
	if(super==NULL)
		return 0;

	for(std::list<Object3D*>::iterator it=super->objects.begin();it!=super->objects.end();++it)
	{
		if( !(*this==**it) && (*it)->intersects(*this))
		{
			(*it)->handleCollision(*this);
			result=true;
		}
	}	
	return result;
}



void Object3D::kill()
	{
		if(super->super!=0)
			super=super->super;
		pos=Vector3D(0,0,-100);
		dir=Vector3D();
	}
bool Object3D::intersects(Object3D& o){
	Vector3D c(center()),oc(o.center());
	Vector3D cmax(center()+(dim*0.5)),omax(o.center()+(o.dim*0.5));
	Vector3D cmin(center()-(dim*0.5)),omin(o.center()-(o.dim*0.5));
	
	if((omax.dx>=cmin.dx && omin.dx<=cmax.dx)&& (omax.dy>=cmin.dy && omin.dy<=cmax.dy) &&(omax.dz>=cmin.dz && omin.dz<=cmax.dz))
   		return true;
	return false;
}

bool operator==(const Object3D& a,const Object3D& b)
{
	if(a.id==b.id)
		return true;
	return false;
}




//**************************SPHERE3D**************************
Sphere3D::Sphere3D(double _bindingRadius,Container3D* containingField,TextureCollection _TX,GLUquadricObj *_quadratic,
				   double _dx,double _dy,double _dz,double _x,double _y,double _z,int _rotspeed,double _speed,int _health)
				   :Object3D(_x,_y,_z,_bindingRadius*2,_bindingRadius*2,_bindingRadius*2,TX),rotspeed(_rotspeed)
{
	bindingRadius=_bindingRadius;
	speed=_speed;
	health=_health;
	quadratic=_quadratic;
	xrot=yrot=20;
	super=containingField;
	TX=_TX;
	health=100;
	dir.dx=_dx;
	dir.dy=_dy;
	dir.dz=_dz;
	dir.normalize();

	
}



bool Sphere3D::intersects(Object3D& b)
{
	double distance = sqrt(pow((pos.dx-b.pos.dx),2)+pow((pos.dy-b.pos.dy),2)+pow((pos.dz-b.pos.dz),2));

	if(distance < bindingRadius + b.bindingRadius )
		return true;
	return false;
}




void Sphere3D::draw()
{
	glBindTexture(GL_TEXTURE_2D,TX.getTX());
	glPushMatrix();
	glTranslatef(pos.dx,pos.dy,pos.dz);
	glRotatef(xrot,1.0f,0.0f,0.0f);
	glRotatef(yrot,0.0f,1.0f,0.0f);
	xrot+=rotspeed;yrot+=rotspeed;


	gluQuadricDrawStyle  (quadratic, GLU_FILL);
	gluQuadricNormals    (quadratic, GLU_FLAT);
	gluQuadricOrientation(quadratic, GLU_OUTSIDE);


	gluSphere(quadratic,bindingRadius,32,32);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D,NULL);

}


void Sphere3D::actOn(Object3D& p){
		p.takeDamage();
};

//**************************CONTAINER3D**************************
void Container3D::addObject(Object3D* o)
{
	o->super=this;
	objects.push_back(o);

}

void Container3D::update()
{
	for(list<Object3D*>::iterator it=objects.begin();it!=objects.end();++it)
			(*it)->update();
}



Container3D::~Container3D()
{
		for(list<Object3D*>::iterator it=objects.begin();it!=objects.end();++it)
				delete (*it);

}

//**************************TOPCONTAINER#D**************************

TopContainer::TopContainer(int _n,double l,double h, double d,TextureCollection _TX,bool db=false)
	:Container3D(0-l/2,0-h/2,0-d/2,l,h,d,_TX)
	{
		//glLoadIdentity();
		n=_n;
		  TX=_TX;
		 draw_borders=!db;
		lStep=l/n;
		hStep=h/n;
		dStep=d/n;
		
		int i=0,j=0,k=0;
	
		for( i=0;i<n;++i)
			for( j=0;j<n;++j)
				for( k=0;k<n;++k)
					sectors.push_back(Container3D(pos.dx+i*lStep,pos.dy+j*hStep,pos.dz+k*dStep,lStep,hStep,dStep,TX,this,db));
	
		//sectors.push_back(Container3D(originX+hStep*i-hStep/8,originY+vStep*j-vStep/8,hStep*1.25,vStep*1.25));
		dim.dx=i*lStep;
		dim.dy=j*hStep;
		dim.dz=k*dStep;

		
	};
void TopContainer::update()
{
	for(list<Object3D*>::iterator it=objects.begin();it!=objects.end();it++) //for all objects in field, distribute to sectors
	{
		if(!(*it)->dead())
		{
			findSectorFor(**it);
			it=objects.erase(it);
			if(it==objects.end())
				break;
		}
	}

	for(int i=0;i<sectors.size();++i)
	{
		sectors[i].update();
		for(list<Object3D*>::iterator it=sectors[i].objects.begin();it!=sectors[i].objects.end();++it)
		{
			if((*it)->dead()){
				it=sectors[i].objects.erase(it);
				if(it==sectors[i].objects.end())
					break;
			
			}
			if(!sectors[i].containsOriginOf(**it))
			{

 				findSectorFor(**it);	
				it=sectors[i].objects.erase(it);
				if(it==sectors[i].objects.end())
					break;
			}
				
		}
	}
}
void TopContainer::draw()
{
	
	Container3D::draw();
	
	for(vector<Container3D>::iterator it=sectors.begin();it!=sectors.end();++it)
	{
		it->draw();
	}
}
void TopContainer::findSectorFor(Object3D& c)
{
	if(c.dir.dx!=c.dir.dx ){//|| c.dir.dy!=c.dir.dy || c.dir.dz!=c.dir.dz || c.pos.dx!=c.dir.dx || c.pos.dy!=c.dir.dy || c.pos.dz!=c.dir.dz){ //#ind
        c.dir=Vector3D(0,0,0);
		c.pos=Vector3D(0,0,0);
		addObject(&c);
		return;
	}

	Vector3D cmax(c.center()+(c.dim*0.5));
	Vector3D cmin(c.center()-(c.dim*0.5));


	vector<Container3D>::iterator it;
	int xI=abs(c.center().dx+c.dim.dx*0.5-pos.dx)/lStep;
	int yI=abs(c.center().dy+c.dim.dy*0.5-pos.dy)/hStep;
	int zI=abs(c.center().dz+c.dim.dz*0.5-pos.dz)/dStep;
	
	
	if(xI>=n) xI=n-1;
	if( yI>=n ) yI=n-1;
	if( zI>=n) zI=n-1;
	//	return *this;
	int index=xI*n*n+yI*n+zI;
	//if(index<0 || index>n*n*n)
	//	addObject(&c);
	
	sectors[index].addObject(&c);


}