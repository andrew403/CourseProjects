#include "character.h"
//#include "ScriptManager.h"
#include <cstring>
//#include "v8helpers.h"
using namespace std;

//vector<Character*> Character::game_objects;

void Character::updateWindowCollision(sf::RenderTarget* target){
    if(this->CharShape.getGlobalBounds().left <= 0.f){
        
        this->CharShape.setPosition(0, this->CharShape.getGlobalBounds().top);
    }
    // Right side
    else if(this->CharShape.getGlobalBounds().left + this->CharShape.getGlobalBounds().width >= target->getSize().x){
        this->CharShape.setPosition(target->getSize().x - this->CharShape.getGlobalBounds().width, this->CharShape.getGlobalBounds().top);
    }
    // Top left
    else if(this->CharShape.getGlobalBounds().left <= 0.f && this->CharShape.getGlobalBounds().top <= 0.f){
        this->CharShape.setPosition(0, 0);
    }
    // Top side
    if(this->CharShape.getGlobalBounds().top <= 0.f ){
        this->CharShape.setPosition((int)this->CharShape.getGlobalBounds().left  , 0.f);
    }
    // Bottom side
    else if(this->CharShape.getGlobalBounds().top + this->CharShape.getGlobalBounds().height >= target->getSize().y){
        this->CharShape.setPosition((int)this->CharShape.getGlobalBounds().left , target->getSize().y - this->CharShape.getGlobalBounds().height);
    }


}

void Character::mvLeft(float spd){
    this->CharShape.move(-1*spd, 0.f);
}

void Character::mvRight(float spd){
    this->CharShape.move(spd, 0.f);
}

void Character::CharRender(sf::RenderTarget* target){

    target->draw(this->CharShape);
}

void Character::updateInput(sf::RenderTarget* Target, float dt){

    //Process keyboard input here
        
        float spd = dt*this->velocity;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) 
            this->CharShape.move(-1*spd, 0.f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            this->CharShape.move(spd, 0.f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
                this->jump();
            //this->CharShape.move(0.f, -1*spd);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            this->CharShape.move(0.f, spd);
            

}

void Character::CharUpdate(sf::RenderTarget* target, float dt){
    this->updateInput(target, dt);
    //Set boundary collision
    //this->updateWindowCollision(target);
    
   
}

float Character::getCharPosX(){
    return this->CharShape.getPosition().x;
}

float Character::getCharPosY(){
    return this->CharShape.getPosition().y;
}

void Character::jump(){
    if(this->isjumping == false){
        this->CharShape.move(0.f, -5);
        this->isjumping = true; // If you don't want the character to jump in the air (i.e. flappy bird mode), disable this line
        this->velocityY=-20;
    }
}

Character::Character(){
    this->spawnX = 200;
    this->spawnY = 100;
    this->CharShape.setPosition(this->spawnX, this->spawnX);
    this->velocity = 500.f;

    this->initChar();
    //this->guid = "gameobject" + std::to_string(this->current_guid);
    //current_guid++;
    //game_objects.push_back(this);

}

Character::~Character(){
}

void Character::initChar(){
    // Import texture
    if(!this->skin.loadFromFile("image/ufo.png")){
        cout << "Fail to load character texture" << endl;
    }
    this->CharShape.setTexture(this->skin);
    this->CharShape.setScale(0.1f, 0.1f);

}


/*void Character::ScriptedCharacterFactory(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate *isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::EscapableHandleScope handle_scope(args.GetIsolate());
	v8::Context::Scope context_scope(context);

	std::string context_name("default");

    if(args.Length() == 1)
	{
		v8::String::Utf8Value str(args.GetIsolate(), args[0]);
		context_name = std::string(v8helpers::ToCString(str));
#if GO_DEBUG
		std::cout << "Created new object in context " << context_name << std::endl;
#endif
	}
	GameObject *new_object = new GameObject();
	v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
	args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
}


v8::Local<v8::Object> Character::exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name) {
    // 类似于 GameObject::exposeToV8 的实现
    std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer("left", getGameObjectX, setGameObjectX));
	v.push_back(v8helpers::ParamContainer("guid", getGameObjectGUID, setGameObjectGUID));
	return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
}

void Character::setGameObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Character*>(ptr)->spawnX = value->Int32Value();
}

void Character::getGameObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x_val = static_cast<Character*>(ptr)->spawnX;
	info.GetReturnValue().Set(x_val);
}

void Character::setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString());
	static_cast<Character*>(ptr)->guid = *utf8_str;
}

void Character::getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	std::string guid = static_cast<Character*>(ptr)->guid;
	v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str(), v8::String::kNormalString);
	info.GetReturnValue().Set(v8_guid);
}*/

