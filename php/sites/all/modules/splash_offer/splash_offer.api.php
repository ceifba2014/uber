<?php
/**
 * @file
 * Hooks provided by this module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Acts on apps being loaded from the database.
 *
 * This hook is invoked during app loading, which is handled by
 * entity_load(), via the EntityCRUDController.
 *
 * @param array $entities
 *   An array of app entities being loaded, keyed by id.
 *
 * @see hook_entity_load()
 */
function hook_splash_offer_load(array $entities) {
  $result = db_query('SELECT pid, foo FROM {mytable} WHERE pid IN(:ids)', array(':ids' => array_keys($entities)));
  foreach ($result as $record) {
    $entities[$record->pid]->foo = $record->foo;
  }
}

/**
 * Responds when a app is inserted.
 *
 * This hook is invoked after the app is inserted into the database.
 *
 * @param SplashOfferEntity $entity
 *   The app that is being inserted.
 *
 * @see hook_entity_insert()
 */
function hook_splash_offer_insert(SplashOfferEntity $entity) {
  db_insert('mytable')
    ->fields(array(
      'id' => entity_id('splash_offer', $entity),
      'extra' => print_r($entity, TRUE),
    ))
    ->execute();
}

/**
 * Acts on a app being inserted or updated.
 *
 * This hook is invoked before the app is saved to the database.
 *
 * @param SplashOfferEntity $entity
 *   The app that is being inserted or updated.
 *
 * @see hook_entity_presave()
 */
function hook_splash_offer_presave(SplashOfferEntity $entity) {
  $entity->name = 'foo';
}

/**
 * Responds to a app being updated.
 *
 * This hook is invoked after the app has been updated in the database.
 *
 * @param SplashOfferEntity $entity
 *   The app that is being updated.
 *
 * @see hook_entity_update()
 */
function hook_splash_offer_update(SplashOfferEntity $entity) {
  db_update('mytable')
    ->fields(array('extra' => print_r($entity, TRUE)))
    ->condition('id', entity_id('splash_offer', $entity))
    ->execute();
}

/**
 * Responds to app deletion.
 *
 * This hook is invoked after the app has been removed from the database.
 *
 * @param SplashOfferEntity $entity
 *   The app that is being deleted.
 *
 * @see hook_entity_delete()
 */
function hook_splash_offer_delete(SplashOfferEntity $entity) {
  db_delete('mytable')
    ->condition('pid', entity_id('splash_offer', $entity))
    ->execute();
}

/**
 * Alter app forms.
 *
 * Modules may alter the {ENTITY} entity form by making use of this hook or
 * the entity bundle specific hook_form_{ENTITY_ID}_edit_BUNDLE_form_alter().
 * #entity_builders may be used in order to copy the values of added form
 * elements to the entity, just as documented for
 * entity_form_submit_build_entity().
 *
 * @param $form
 *   Nested array of form elements that comprise the form.
 * @param $form_state
 *   A keyed array containing the current state of the form.
 */
function hook_form_splash_offer_form_alter(&$form, &$form_state) {
  // Your alterations.
}

/**
 * @}
 */
