/** \file
 */

#pragma once

#include <BsPrerequisites.h>
#include <BsZenLib/ZenResources.hpp>
#include <Scene/BsComponent.h>

namespace REGoth
{
  /**
   * Component for rendering an animated character (Player, NPC, Monster).
   *
   * This component will take care of loading the correct resources and has
   * an interface to play animations. It can also be configured such that
   * the root-motion of the played animation is applied to the scene object.
   *
   * # Object tree
   *
   * Because of the way bsf works, one cannot modify any transforms like scaling
   * or rotation if root-motion is used on a scene object. Therefore the actual
   * characters renderable and animation components are put into a sub-object.
   *
   * # Model script files
   *
   * In Gothic, similar creatures and characters are described inside a so called
   * *model script file*. Inside this file, every mesh matching the animation
   * node tree is listed, along with all possible animations and their properties.
   *
   * Therefore, this component needs to know the model script and the mesh
   * that it should display from that model script.
   */
  class VisualCharacter : public bs::Component
  {
  public:
    VisualCharacter(const bs::HSceneObject& parent);

    /**
     * Set which model script this component should read the list of possible
     * meshes and animations from. This **must** be set for the component to work.
     */
    void setModelScript(BsZenLib::Res::HModelScriptFile modelScript);

    /**
     * Set the mesh this component should display. This mesh **must** come from
     * within the given model script file, otherwise the component won't display
     * the mesh.
     */
    void setMesh(BsZenLib::Res::HMeshWithMaterials mesh);

    /**
     * Calculates how far the characters animation has moved since the last
     * call ot this function via animation.
     *
     * @return Delta between the animation postion when this function was called
     *         the last time and now.
     */
    bs::Vector3 resolveFrameRootMotion();

    /**
     * @return The bounds of the underlaying renderable
     */
    bs::Bounds getBounds() const;

    /**
     * Find the animation clip matching the currently set model script and overlay.
     * Returns an invalid handle when the was not found.
     *
     * @param  name  The UPPERCASE animation name (`S_RUNL`)
     *
     * @return Animation clip for the given animation name. Invalid if not found.
     */
    bs::HAnimationClip findAnimationClip(const bs::String& name) const;

    /**
     * Plays the given animation clip.
     */
    void playAnimation(bs::HAnimationClip clip);

    /**
     * Tries to transition to the given animation name.
     *
     * @param  Animation to transition to, eg `S_RUNL` or `T_JUMPB`.
     *
     * @return true, if the transition was possible.
     */
    bool tryPlayTransitionAnimationTo(const bs::String& state);

    /**
     * @return Whether the given animation is currently playing
     */
    bool isAnimationPlaying(bs::HAnimationClip clip) const;

    /**
     * @return Name of the currently playing animation. Empty string if none.
     */
    bs::String getPlayingAnimationName() const;

    /**
     * @return The current state animations state, eg. `RUNL` for `S_RUNL`.
     */
    bs::String getStateFromPlayingAnimation() const;

    /**
     * Whether the currently playing animation can be interrupted by the user.
     *
     * This will return false on animations like jumping backwards or
     * falling onto the floor when dying.
     *
     * If no animation is played, true is returned.
     *
     * @return Whether the user can interrupt the currently playing animation.
     */
    bool isPlayingAnimationInterruptable() const;

  private:
    /**
     * @return Whether the given mesh is registered inside the currently set model script
     */
    bool isMeshRegisteredInModelScript(BsZenLib::Res::HMeshWithMaterials mesh);

    /**
     * Deletes all sub objects created by this component (ie. for rendering)
     */
    void deleteObjectSubtree();

    /**
     * (Re)creates all sub objects needed ie. for rendering.
     */
    void buildObjectSubtree();

    /**
     * Creates the sub object and all components needed on that object.
     */
    void createAndRegisterSubComponents();

    /**
     * Creates a sub-object and adds it to the list of all sub-objects.
     */
    bs::HSceneObject createAndRegisterSubObject(const bs::String& name);

    /**
     * Sets up the empty previously created renderable component.
     */
    void setupRenderableComponent();

    /**
     * Sets up the empty previously create animation component.
     */
    void setupAnimationComponent();

    /**
     * Called when a animation event was triggered
     */
    void onAnimationEvent(const bs::HAnimationClip& clip, bs::String string);

    // Object Sub Tree --------------------------------------------------------
    bs::Vector<bs::HSceneObject> mSubObjects; /**< All created sub-objects by this component */

    bs::HRenderable mSubRenderable; /**< The Renderable created inside a sub object */
    bs::HAnimation mSubAnimation;   /**< The Animation-Component created inside a sub object */

    // Configuration ----------------------------------------------------------

    BsZenLib::Res::HModelScriptFile mModelScript; /**< Model-script of the displayed model */
    BsZenLib::Res::HMeshWithMaterials mMesh; /**< Currently displayed mesh, from the model script */

    // Animation --------------------------------------------------------------
    bs::Map<bs::String, bs::HAnimationClip> mAnimationClips; /**< Animation names -> clip */
    bs::HAnimationClip mRootMotionLastClip; /**< Last clip we got the root motion from */
    float mRootMotionLastTime; /**< Last time the animation was queried for root motion */
  };

  using HVisualCharacter = bs::GameObjectHandle<VisualCharacter>;
}  // namespace REGoth
