<?php

/**
 * Common mappings for the Drupal 7 node migrations.
 */
abstract class EfNodeMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_legacy_nid', 'nid')
      ->description('We have a common field to save the D7 nid');
  }
}

/**
 * Image-specific mappings and handling.
 */
class FestivalsMigration extends EfNodeMigration {

  public function __construct(array $arguments) {

    parent::__construct($arguments);

    $this->addSimpleMappings(array(
      'field_add_thisf',
      'field_age',
      'field_confirmed',
      'field_date',
      'field_facebook_event',
      'field_facebook_event:title',
      'field_facebook_event:attributes',
      'field_facebook_event:language',
      'field_facebook_promoter',
      'field_facebook_promoter:title',
      'field_facebook_promoter:attributes',
      'field_facebook_promoter:language',
      'field_festicket_logo',
      'field_festicket_packages',
      'field_festicket_shop',
      'field_festicket_shop:format',
      'field_festicket_url',
      'field_festival_classification',
      'field_gemischt',
      'field_genre_tags',
      'field_location',
      'field_visitors',
      'field_indoor',
      'field_type_of_event',
      'field_ticket_price',
      'field_youtube_event',
      'field_youtube_event:title',
      'field_youtube_event:attributes',
      'field_youtube_event:language',
      'field_url_event',
      'field_url_event:title',
      'field_url_event:attributes',
      'field_url_event:language',
      'field_url_promoter',
      'field_url_promoter:title',
      'field_url_promoter:attributes',
      'field_url_promoter:language',
      'field_lineup',
      'field_lineup:format',
      'field_youtube_promoter',
      'field_youtube_promoter:title',
      'field_youtube_promoter:attributes',
      'field_youtube_promoter:language',
      'field_trailer',
      'field_trailer:display',
      'field_trailer:description',
      'field_lineup_tags',

    ));
    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('field_festicket_packages:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('field_festicket_packages:create_term')
      ->defaultValue(FALSE);
    $this->addFieldMapping('field_festicket_packages:ignore_case')
      ->defaultValue(FALSE);
    $this->addFieldMapping('field_genre_tags:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_genre_tags:create_term')->defaultValue(FALSE);
    $this->addFieldMapping('field_genre_tags:ignore_case')->defaultValue(FALSE);
    $this->addFieldMapping('field_lineup_tags:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_lineup_tags:create_term')->defaultValue(FALSE);
    $this->addFieldMapping('field_lineup_tags:ignore_case')->defaultValue(FALSE);

    $this->addUnmigratedSources(array(
      // 'body:language',
      'field_location:language',
      'field_date:value2',
      'field_date:language',
      'field_visitors:language',
      'field_indoor:language',
      'field_ticket_price:language',
      'field_type_of_event:language',
      'field_age:language',
      'field_add_thisf:language',
      'field_confirmed:language',
      'field_festicket_shop:language',
      'field_festival_classification:language',
      'field_lineup_tags:language',
      'field_genre_tags:language',
      'field_gemischt:language',
      'field_trailer:language',
      'field_lineup:language',
    ));

    $this->addUnmigratedDestinations(array(
      'field_date:rrule',
      'field_date:timezone',
      'field_date:to',
      'field_festicket_url:title',
      'field_festicket_url:attributes',
      'field_festicket_url:language',
    ));


    // $this->addFieldMapping('revision')->defaultValue(FALSE);


  }

  /**
   * Implementation of Migration::prepareRow().
   *
   * Here we pull in additional data from a non-core, non-CCK table.
   *
   * @param $row
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $image_row = Database::getConnection('default', $this->sourceConnection)
      ->select('custom_image_table', 'i')
      ->fields('i', array('copyright', 'filename'))
      ->condition('vid', $row->vid)
      ->execute()
      ->fetchObject();
    if ($image_row) {
      $row->copyright = $image_row->copyright;
      $row->filename = $image_row->filename;
    }
    else {
      $this->saveMessage(t('No custom_image_table data for image node with vid !vid',
        array('!vid' => $row->vid)));
    }
  }
}


class OldLineupsMigration extends EfNodeMigration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.
    $this->sourceFields['lead_graphic'] = 'Lead graphic string from D6';
    $this->sourceFields['summary'] = 'Summary string from D6';

    parent::__construct($arguments);

    $this->addFieldMapping('field_image', 'field_lead_graphic')
      ->sourceMigration('Image');
    // Note that we map migrated terms by the vocabulary ID.
    $this->addFieldMapping('field_countries', '1')
      ->sourceMigration('Countries');
    $this->addFieldMapping('field_countries:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('field_topics', '2')
      ->sourceMigration('Topics');
    $this->addFieldMapping('field_topics:source_type')
      ->defaultValue('tid');
  }
}