<?php
/**
 * Stripped out all unncessary markup as assets are nested within other areas.
 */
?>
<div class="<?php print $classes; ?>"<?php print $attributes; ?>>

<?php if (!$label_hidden): ?>
  <div class="field-label"<?php print $title_attributes; ?>><?php print $label ?>:&nbsp;</div>
<?php endif; ?>

<?php if(count($items) > 1): ?>
  <div class="field-items"<?php print $content_attributes; ?>>
<?php endif; ?>

<?php foreach ($items as $delta => $item): ?>
  <?php if(count($items) > 1): ?>
    <div class="field-item <?php print $delta % 2 ? 'odd' : 'even'; ?>"<?php print $item_attributes[$delta]; ?>>
  <?php endif; ?>

    <?php print render($item); ?>

  <?php if(count($items) > 1): ?>
    </div>
  <?php endif; ?>
<?php endforeach; ?>

<?php if(count($items) > 1): ?>
  </div>
<?php endif; ?>

</div>
