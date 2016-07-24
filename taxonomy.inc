<?php

/**
 * @file
 * Implementation of DrupalTerm7Migration for Drupal 7 terms sources.
 */
abstract class EfTermMigration extends DrupalTerm7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $elements = metatag_get_info();

    foreach ($elements['tags'] as $value) {
      $metatag_field = 'metatag_' . $value['name'];
      $this->sourceFields[$metatag_field] = $value['description'];
      $simple_mappings[] = $metatag_field;
    }

    $this->source = new MigrateSourceSQL($this->query(), $this->sourceFields);

    $this->addFieldMapping('field_legacy_tid', 'tid')
      ->description('We have a common field to save the D7 tid');
    $this->addUnmigratedSources(array('data'));
    $this->addSimpleMappings($simple_mappings);
  }

  public function prepareRow($row) {
    parent::prepareRow($row);

    if (isset($row->data) && !empty($row->data)) {
      $data = unserialize($row->data);
      foreach ($data as $k => $item) {
        $row->{'metatag_' . $k} = $item;
      }
    }

  }

  protected function query() {
    $query = parent::query();
    $query->leftJoin('metatag', 'm', 'm.entity_id = td.tid');
    $query->addField('th', 'parent', 'parent');
    $query->fields('m', array('data'));
    return $query;
  }
}

/**
 * Class GenreTermMigration
 *
 * Migration for Genre terms.
 */
class GenreTermMigration extends EfTermMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addUnmigratedDestinations(array(
      'field_url_safe_name',
      // Ignore field_genre as we are later fetching the reference using GenreUpdateTermMigration.
      'field_genre',
      'field_genre:source_type',
      'field_genre:create_term',
      'field_genre:ignore_case',
    ));
    $this->addUnmigratedSources(array('field_genre', 'field_genre:language'));
  }

  public function prepareRow($row) {
    parent::prepareRow($row);
  }

  protected function query() {
    $query = parent::query();
    return $query;
  }
}

/**
 * Class DjTermMigration
 *
 * Migration for Dj taxonomy terms.
 */
class DjTermMigration extends EfTermMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addSimpleMappings(
      array(
        'field_homepage_act_link',
        'field_homepage_act_link:title',
        'field_homepage_act_link:attributes',
        'field_homepage_act_link:language',
        'field_facebook_link',
        'field_facebook_link:title',
        'field_facebook_link:attributes',
        'field_facebook_link:language',
        'field_youtube_act_link',
        'field_youtube_act_link:title',
        'field_youtube_act_link:attributes',
        'field_youtube_act_link:language',
        'field_soundcloud_link',
        'field_soundcloud_link:title',
        'field_soundcloud_link:attributes',
        'field_soundcloud_link:language',
        'field_twitter_act_link',
        'field_twitter_act_link:title',
        'field_twitter_act_link:attributes',
        'field_twitter_act_link:language',
        'field_itunes_act_link',
        'field_itunes_act_link:title',
        'field_itunes_act_link:attributes',
        'field_itunes_act_link:language',
        'field_residentadvisor_link',
        'field_residentadvisor_link:title',
        'field_residentadvisor_link:attributes',
        'field_residentadvisor_link:language',
        'field_vk_link',
        'field_vk_link:title',
        'field_vk_link:attributes',
        'field_vk_link:language',
        'field_lastfm_link',
        'field_lastfm_link:title',
        'field_lastfm_link:attributes',
        'field_lastfm_link:language',
        'field_mixcloud_link',
        'field_mixcloud_link:title',
        'field_mixcloud_link:attributes',
        'field_mixcloud_link:language',
        'field_bearbeitet_von',
        'field_genredj',
        'field_bandsintown_link',
        'field_bandsintown_link:title',
        'field_bandsintown_link:attributes',
        'field_bandsintown_link:language',

      )
    );
    $this->addFieldMapping('field_genredj:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_genredj:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_genredj:ignore_case')->defaultValue(TRUE);

    $this->addUnmigratedSources(array(
      'field_genredj:language',
      'field_bearbeitet_von:language',
    ));

  }

  public function prepareRow($row) {
    parent::prepareRow($row);
    if (!empty($row->field_genredj)) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'taxonomy_term')
        ->entityCondition('bundle', 'music_genre')
        ->fieldCondition('field_legacy_tid', 'value', $row->field_genredj);
      $result = $query->execute();
      if (isset($result['taxonomy_term'])) {
        $tid = array_keys($result['taxonomy_term']);
        $row->field_genredj = array_shift($tid);
      }
    }
  }

  protected function query() {
    $query = parent::query();
    return $query;
  }
}

/**
 * Class FesticketPackagesTerm7Migration
 *
 * Migration for FesticketPackages taxonomy terms.
 */
class FesticketPackagesTerm7Migration extends EfTermMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addSimpleMappings(array(
      'field_ft_package_url',
      'field_ft_package_image',
    ));

    $this->addUnmigratedDestinations(array(
      'field_ft_package_url:title',
      'field_ft_package_url:attributes',
      'field_ft_package_url:language',
    ));
  }

  public function prepareRow($row) {
    parent::prepareRow($row);
  }

  protected function query() {
    $query = parent::query();
    return $query;
  }
}

/**
 * Class UpdateGenreTermMigration
 *
 * Migration class to update Genre taxonomy term field Genre with Dj term
 * references.
 */
class UpdateGenreTermMigration extends EfTermMigration {
  /**
   * Indicate whether the primary system of record for this migration is the
   * source, or the destination (Drupal). In the source case, migration of
   * an existing object will completely replace the Drupal object with data from
   * the source side. In the destination case, the existing Drupal object will
   * be loaded, then changes from the source applied; also, rollback will not be
   * supported.
   *
   * @var int
   */
  const SOURCE = 1;
  const DESTINATION = 2;

  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->systemOfRecord = Migration::DESTINATION;
    $this->removeFieldMapping('field_legacy_tid');

    $this->addFieldMapping('tid', 'tid')->sourceMigration('Genre');
    $this->addSimpleMappings(array('field_genre'));
    $this->addFieldMapping('field_genre:source_type')->defaultValue(TRUE);
    $this->addFieldMapping('field_genre:create_term')->defaultValue(FALSE);
    $this->addFieldMapping('field_genre:ignore_case')->defaultValue(TRUE);
    $this->addUnmigratedDestinations(array(
      'field_url_safe_name',
      'field_legacy_tid',
    ));
    $this->addUnmigratedSources(array('field_genre:language'));
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param $row
   * @return bool
   */
  public function prepareRow($row) {
    parent::prepareRow($row);
    if (!empty($row->field_genre)) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'taxonomy_term')
        ->entityCondition('bundle', 'dj')
        ->fieldCondition('field_legacy_tid', 'value', $row->field_genre);
      $result = $query->execute();

      if ($result['taxonomy_term']) {
        $row->field_genre = array_shift(array_keys($result['taxonomy_term']));
      }
    }
  }

  protected function query() {
    $query = parent::query();
    return $query;
  }
}