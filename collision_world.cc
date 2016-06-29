
// This is the collision world instantiated by the user.
// The actual implementation is completely hidden.
// There are no Bullet or FCL includes here.
class CollisionWorld { // just a name change for DrakeCollision::Model
public:
  CollisionWorld() {
#ifdef BULLET_COLLISION
    pimpl_.reset(new BulletCollisionWorld());
#elif FCL_COLLISION
    pimpl_.reset(new FCLCollisionWorld());
#else
    // ... some sort of static assert ...
#endif
  }
  
  // The CollisionWorld (not the implementation) takes ownership of the CollisionElement.
  // In turn, CollisionElement has ownership of its own implmentation.
  void add_collision_element(std::unique_ptr<CollisionElement> element) {
    pimpl_->add_collision_element(element.get());     // this call does not take ownership.
    collision_objects_.push_back(std::move(element)); // take ownership here.
  }

  void ComputeMaximumDepthCollisionPoints() {
    pimpl_->ComputeMaximumDepthCollisionPoints();
  }
  
private:
  std::vector<std::unique_ptr<CollisionObject>> collision_objects_; 
  std::unique_ptr<CollisionWorldImpl> pimpl_;
};

// Base (abstract) class for implementations.
class CollisionWorldImpl {
public:
  virtual void add_collision_element(CollisionElement* element) = 0;  
  virtual void ComputeMaximumDepthCollisionPoints() = 0;
};

// This will live in its own header + cc files with the appropiate Bullet includes.
class BulletCollisionWorld: public CollisionWorldImpl {
public:
  BulletCollisionWorld() { 
    // ... Construction and Bullet's world setup here ...
  }
  void add_collision_element(CollisionElement* element) override {
    collision_objects_.push_back(dynamic_cast<BulletCollisionElement>(CollisionElement));
  }
  
  // Some collision dispatch method.
  void ComputeMaximumDepthCollisionPoints() override {
    // ... Everythin inside this method can be performed (in a clean way) with Bullet's objects.
    // No dynamic castings or id's around.
  }

private:
  // No need for id's!! we can store the atual collision object implementation 
  // for the particular world implmentation. 
  // No nasty integer id's.
  // No readElement calls.
  // No dynamic_cast's all over the place.
  std::vector<BulletCollisionElement*> collision_objects_; 

  // As you can see it can get quite complex for a specific implementation.
  // So very worth encapsulating it into a simple implementatin object.
  btDefaultCollisionConfiguration bt_collision_configuration;
  btDbvtBroadphase bt_collision_broadphase;
  OverlapFilterCallback filter_callback;
  std::unique_ptr<btCollisionDispatcher> bt_collision_dispatcher;
  std::unique_ptr<btCollisionWorld> bt_collision_world;
}

// Similarly for an FCL collision world implementation ...
// This will live in its own header + cc files with the appropiate FCL includes.
class FCLCollisionWorld: public CollisionWorldImpl {
  // ... FCL specific code here ...
};

