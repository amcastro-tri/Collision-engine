
class CollisionElement { // DrakeCollision::Element right now
public:
  CollisionElement(const DrakeShapes::Geometry& geometry, const Eigen::Isometry3d& T_geo_to_element) { 
    // This is right now nastily managed by DrakeCollision::newModel()
#ifdef BULLET_COLLISION
    pimpl_.reset(new BulletCollisionElement(geometry, T_geo_to_element));
#elif FCL_COLLISION
    pimpl_.reset(new FCLCollisionElement(geometry, T_geo_to_element));
#endif
  }

  void update_geometry_to_element_transform(const Eigen::Isometry3d& T) { 
    pimpl_->update_geometry_to_element_transform(T); 
  }

  // Factory method provided so that users do not ever see a "new".
  // CollsionWorld here is nothing but DrakeCollision::Model
  // (the "model" world is used too much in drake and this change won't brake any API)
  static CollisionElement* CreatedAndAddToCollisionWorld(const CollisionWorld& world, const DrakeShapes::Geometry& geometry, const Eigen::Isometry3d& T_geo_to_element) { 
    CollisionElement* element{nullptr};
    std::unique_ptr<CollisionElement> owned_element(element = new CollisionElement(geometry, T_geo_to_element));
    world.add_collision_element(owned_element);
    return element; // Returns a pointer in case the user wants to do something like element->update_transform(T) or element->set_static(), no id's!!
  }

private:
  std::unique_ptr<CollisionElementImpl> pimpl_;
};

class CollisionElementImpl {
public:
  virtual void add_to_collision_clique(int clique) = 0;
  virtual void update_geometry_to_element_transform(const Eigen::Isometry3d& T) = 0;
  virtual ~CollisionElementImpl() {}
};

// A particular implementation for Bullet collision elements
class BulletCollisionElement: public CollisionElementImpl {
public:
  BulletCollisionElement(const DrakeShapes::Geometry& geometry, const Eigen::Isometry3d& T_geo_to_element) { 
    // ... Here either use something similar to what happens right now in BulletModel::addElement (nasty) ...
    // ... or use double dispatch to get the right derived Bullet object created ..
    bt_obj_ = /*...*/
  }

  void update_geometry_to_element_transform(const Eigen::Isometry3d& T) override {
    btTransform btT;
    // .. code to go from Eigen T to Bullet's btT
    bt_obj_->setWorldTransform(btT); 
  }
private:
  std::unique_ptr<btCollisionObject> bt_obj_;  
};

// A particular implementation for FCL collision elements
class FCLCollisionElement: public CollisionElementImpl {
public:
  FCLCollisionElement(const DrakeShapes::Geometry& geometry, const Eigen::Isometry3d& T_geo_to_element) { 
    // ... Here either use something similar to what happens right now in BulletModel::addElement (nasty) ...
    // ... or use double dispatch to get the right derived Bullet object created ...
    fcl_obj_ = /*...*/
  }

  void update_geometry_to_element_transform(const Eigen::Isometry3d& T) override {
    // ... FCL specific vudu to set transformation ...
    fcl_obj_->setWorldTransform(btT); 
  }
private:
  std::unique_ptr<FCL::CollisionObject> fcl_obj_;  
};

// The collision engine API then looks like ...
int main() {
  CollisionWorld collision_world;

  DrakeShapes::Box box(Vector3d(1,1,1));
  CollisionElement* colliding_box = CreatedAndAddToCollisionWorld(collision_world, box, Isometry3d::Identity());

  // No nasty id's needed to access my collision element!
  colliding_box->add_to_collision_clique(3);

  // Cleans up properly.
  return 0;
}
