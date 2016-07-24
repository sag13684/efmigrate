<?php

/**
 * @file efmigrate.migrate.inc
 */

/**
 * This is code for electronic festival Drupal 7 to Drupal 7 migration. This won't actually
 * work without defining the referenced vocabularies, content types, and fields
 * on each side.
 */

/**
 * Implements hook_migrate_api().
 */
function efmigrate_migrate_api() {
  /**
   * Declare the api version and migration group.
   */
  $api = array(
    'api' => 2,
    'groups' => array(
      'ef_d7_d7' => array(
        'title' => t('Electronics Festival Drupal 7 to Drupal 7 migrations.'),
      ),
    ),
  );

  /**
   * Each migration being registered takes an array of arguments, some required
   * and some optional. Start with the common arguments required by all - the
   * source_connection (connection key, set up in settings.php, pointing to
   * the Drupal 7 database), source_version (major version of Drupal), and
   * group_name (a.k.a. import job).
   */
  $common_arguments = array(
    'source_connection' => 'legacy',
    'source_version' => 7,
    'group_name' => 'ef_d7_d7',
  );

  // For vocabulary migrations, source_vocabulary and destination_vocabulary are
  // required arguments.
  $vocabulary_arguments = array(
    'FesticketPackages' => array(
      'description' => t('Migration of Festicket Packages from Drupal 7'),
      'class_name' => 'FesticketPackagesTerm7Migration',
      'source_vocabulary' => 'festicket_packages',  // D7 Vocabulary name
      'destination_vocabulary' => 'festicket_packages',
    ),
    'Genre' => array(
      'description' => t('Migration of Genre terms from Drupal 7'),
      'class_name' => 'GenreTermMigration',
      'source_vocabulary' => 'music_genre',  // D7 Vocabulary name
      'destination_vocabulary' => 'music_genre',
    ),
    'Dj' => array(
      'description' => t('Migration of Festicket Packages from Drupal 7'),
      'class_name' => 'DjTermMigration',
      'source_vocabulary' => 'dj',  // D7 vocabulary name
      'destination_vocabulary' => 'dj',
      'dependencies' => array('Genre'),
    ),
    'UpdateGenre' => array(
      'description' => t('Update Genre terms from Drupal 7 to Map Dj terms'),
      'class_name' => 'UpdateGenreTermMigration',
      'source_vocabulary' => 'music_genre',  // D7 Vocabulary name
      'destination_vocabulary' => 'music_genre',
      'dependencies' => array('Genre', 'Dj'),
    ),
  );


  foreach ($vocabulary_arguments as $migration_name => $arguments) {
    $arguments += $common_arguments;
    $api['migrations'][$migration_name] = $arguments;
  }

  // Node migrations - each has its own class derived from the migrate_d2d class,
  // specifying its particular field mappings and transformations. source_type
  // and destination_type are required arguments.
  $node_arguments = array(
    'Festivals' => array(
      'class_name' => 'FestivalsMigration',
      'description' => t('Migration of festival nodes from Drupal 7'),
      'source_type' => 'festivals',
      'destination_type' => 'festivals',
      'dependencies' => array(
        'FesticketPackages',
        'Dj',
        'Genre',
        'UpdateGenre'
      ),
    ),
    /*
    'OldLineups' => array(
      'class_name' => 'OldLineupsMigration',
      'description' => t('Migration of article nodes from Drupal 6'),
      'source_type' => 'old_lineups',
      'destination_type' => 'old_lineups',
      'dependencies' => array('FesticketPackages', 'Dj', 'Genre', 'Festivals'),
    ),
    */
  );

  foreach ($node_arguments as $migration_name => $arguments) {
    $arguments += $common_arguments;
    $api['migrations'][$migration_name] = $arguments;
  }
  return $api;
}
