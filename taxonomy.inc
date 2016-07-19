<?php

/**
 * @file
 * Implementation of DrupalTerm7Migration for Drupal 7 sources.
 */
class GenreTermMigration extends DrupalTerm7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    /*$this->addFieldMapping('field_genre', 'field_genre');
    $this->addFieldMapping('field_genre:source_type')->defaultValue(TRUE);
    $this->addFieldMapping('field_genre:create_term')->defaultValue(FALSE);
    $this->addFieldMapping('field_genre:ignore_case')->defaultValue(TRUE);
    */
    $this->addFieldMapping('field_legacy_tid', 'tid')
      ->description(t('Legacy or Source tid'));
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

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param $row
   * @return bool
   */
  public function prepareRow($row) {
    parent::prepareRow($row);
  }

  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('taxonomy_term_data', 'td')
      ->fields('td', array('tid', 'name', 'description', 'weight', 'format'))
      ->fields('th', array('parent'))
      ->orderBy('th.parent')
      ->distinct();
    // Join to the hierarchy so we can sort on parent, but we'll pull the
    // actual parent values in separately in case there are multiples.
    $query->leftJoin('taxonomy_term_hierarchy', 'th', 'td.tid=th.tid');
    $query->innerJoin('taxonomy_vocabulary', 'v', 'td.vid=v.vid');
    $query->condition('v.machine_name', array($this->sourceVocabulary), 'IN');
    return $query;
  }
}


class DjTermMigration extends DrupalTerm7Migration {
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
    $this->addFieldMapping('field_legacy_tid', 'tid')
      ->description(t('Legacy or Source tid'));
    $this->addUnmigratedSources(array(
      'field_genredj:language',
      'field_bearbeitet_von:language',
    ));

  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param $row
   * @return bool
   */
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
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('taxonomy_term_data', 'td')
      ->fields('td', array('tid', 'name', 'description', 'weight', 'format'))
      ->fields('th', array('parent'))
      ->orderBy('th.parent')
      ->distinct();
    // Join to the hierarchy so we can sort on parent, but we'll pull the
    // actual parent values in separately in case there are multiples.
    $query->leftJoin('taxonomy_term_hierarchy', 'th', 'td.tid=th.tid');
    $query->innerJoin('taxonomy_vocabulary', 'v', 'td.vid=v.vid');
    $query->condition('v.machine_name', array($this->sourceVocabulary), 'IN');
    return $query;
  }
}

class FesticketPackagesTerm7Migration extends DrupalTerm7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addSimpleMappings(array(
      'field_ft_package_url',
      'field_ft_package_image',
    ));
    $this->addFieldMapping('field_legacy_tid', 'tid')
      ->description(t('Legacy or Source tid'));
    $this->addUnmigratedDestinations(array(
      'field_ft_package_url:title',
      'field_ft_package_url:attributes',
      'field_ft_package_url:language',
    ));
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param $row
   * @return bool
   */
  public function prepareRow($row) {
    parent::prepareRow($row);
  }

  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('taxonomy_term_data', 'td')
      ->fields('td', array('tid', 'name', 'description', 'weight', 'format'))
      ->fields('th', array('parent'))
      ->orderBy('th.parent')
      ->distinct();
    // Join to the hierarchy so we can sort on parent, but we'll pull the
    // actual parent values in separately in case there are multiples.
    $query->leftJoin('taxonomy_term_hierarchy', 'th', 'td.tid=th.tid');
    $query->innerJoin('taxonomy_vocabulary', 'v', 'td.vid=v.vid');
    $query->condition('v.machine_name', array($this->sourceVocabulary), 'IN');
    return $query;
  }
}

class UpdateGenreTermMigration extends DrupalTerm7Migration {
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
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('taxonomy_term_data', 'td')
      ->fields('td', array('tid', 'name', 'description', 'weight', 'format'))
      ->fields('th', array('parent'))
      ->orderBy('th.parent')
      ->distinct();
    // Join to the hierarchy so we can sort on parent, but we'll pull the
    // actual parent values in separately in case there are multiples.
    $query->leftJoin('taxonomy_term_hierarchy', 'th', 'td.tid=th.tid');
    $query->innerJoin('taxonomy_vocabulary', 'v', 'td.vid=v.vid');
    $query->condition('v.machine_name', array($this->sourceVocabulary), 'IN');
    return $query;
  }
}